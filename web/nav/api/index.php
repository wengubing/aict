<?php

declare(strict_types=1);

const DATA_FILE = __DIR__ . '/../nav-data.json';

header('Content-Type: application/json; charset=utf-8');

function send_json(int $status, array $payload): void
{
    http_response_code($status);
    echo json_encode($payload, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT);
    exit;
}

function parse_path(): string
{
    $pathInfo = $_SERVER['PATH_INFO'] ?? '';
    if ($pathInfo !== '') {
        return $pathInfo;
    }

    $uriPath = parse_url($_SERVER['REQUEST_URI'] ?? '/', PHP_URL_PATH) ?: '/';
    $scriptName = $_SERVER['SCRIPT_NAME'] ?? '';

    if ($scriptName !== '' && str_starts_with($uriPath, $scriptName)) {
        $rest = substr($uriPath, strlen($scriptName));
        return $rest === '' ? '/' : $rest;
    }

    return '/';
}

function make_id(string $prefix): string
{
    return $prefix . '_' . base_convert((string) time(), 10, 36) . '_' . substr(bin2hex(random_bytes(4)), 0, 6);
}

function ensure_data_file(): void
{
    if (is_file(DATA_FILE)) {
        return;
    }

    $seed = [
        'drawers' => [
            [
                'id' => 'drawer_default',
                'name' => '工作',
                'color' => '#0b5d76',
                'sites' => [
                    [
                        'id' => 'site_github',
                        'url' => 'https://github.com',
                        'title' => 'GitHub',
                        'icon' => 'https://github.githubassets.com/favicons/favicon.svg',
                    ],
                    [
                        'id' => 'site_stackoverflow',
                        'url' => 'https://stackoverflow.com',
                        'title' => 'Stack Overflow',
                        'icon' => 'https://cdn.sstatic.net/Sites/stackoverflow/Img/favicon.ico',
                    ],
                ],
            ],
        ],
    ];

    file_put_contents(DATA_FILE, json_encode($seed, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT), LOCK_EX);
}

function read_data(): array
{
    ensure_data_file();
    $raw = file_get_contents(DATA_FILE);
    if ($raw === false) {
        throw new RuntimeException('读取数据文件失败');
    }

    $parsed = json_decode($raw, true);
    if (!is_array($parsed) || !isset($parsed['drawers']) || !is_array($parsed['drawers'])) {
        return ['drawers' => []];
    }

    return $parsed;
}

function write_data(array $data): void
{
    $ok = file_put_contents(DATA_FILE, json_encode($data, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT), LOCK_EX);
    if ($ok === false) {
        throw new RuntimeException('写入数据文件失败，请检查目录写权限');
    }
}

function read_json_body(): array
{
    $raw = file_get_contents('php://input');
    if ($raw === false || trim($raw) === '') {
        return [];
    }

    $decoded = json_decode($raw, true);
    if (!is_array($decoded)) {
        throw new InvalidArgumentException('JSON 请求体无效');
    }

    return $decoded;
}

function sanitize_url(string $input): string
{
    $trimmed = trim($input);
    if ($trimmed === '') {
        throw new InvalidArgumentException('URL 不能为空');
    }

    if (!preg_match('/^https?:\/\//i', $trimmed)) {
        $trimmed = 'https://' . $trimmed;
    }

    $parts = parse_url($trimmed);
    if (!is_array($parts) || !isset($parts['scheme']) || !isset($parts['host'])) {
        throw new InvalidArgumentException('URL 格式不正确');
    }

    $scheme = strtolower((string) $parts['scheme']);
    if ($scheme !== 'http' && $scheme !== 'https') {
        throw new InvalidArgumentException('仅支持 http/https URL');
    }

    return $trimmed;
}

function fallback_icon(string $url): string
{
    $host = parse_url($url, PHP_URL_HOST) ?: '';
    return 'https://www.google.com/s2/favicons?domain=' . rawurlencode((string) $host) . '&sz=128';
}

function fetch_preview(string $inputUrl): array
{
    $url = sanitize_url($inputUrl);
    $host = (string) (parse_url($url, PHP_URL_HOST) ?: $url);
    $title = $host;
    $icon = fallback_icon($url);

    $ctx = stream_context_create([
        'http' => [
            'method' => 'GET',
            'timeout' => 5,
            'header' => "User-Agent: Mozilla/5.0 (compatible; NavBot-PHP/1.0)\r\n",
            'follow_location' => 1,
            'max_redirects' => 5,
        ],
        'ssl' => [
            'verify_peer' => false,
            'verify_peer_name' => false,
        ],
    ]);

    $html = @file_get_contents($url, false, $ctx);
    if (is_string($html) && $html !== '') {
        if (preg_match('/<title[^>]*>([\s\S]*?)<\/title>/i', $html, $m) === 1) {
            $text = trim(preg_replace('/\s+/', ' ', $m[1] ?? '') ?? '');
            if ($text !== '') {
                $title = mb_substr($text, 0, 80);
            }
        }

        if (preg_match('/<link[^>]+rel=["\'][^"\']*icon[^"\']*["\'][^>]*>/i', $html, $linkTag) === 1) {
            if (preg_match('/href=["\']([^"\']+)["\']/i', $linkTag[0], $hrefMatch) === 1) {
                $href = trim($hrefMatch[1]);
                if ($href !== '') {
                    if (preg_match('/^https?:\/\//i', $href) === 1) {
                        $icon = $href;
                    } else {
                        $base = rtrim($url, '/');
                        if (str_starts_with($href, '/')) {
                            $scheme = (string) parse_url($url, PHP_URL_SCHEME);
                            $domain = (string) parse_url($url, PHP_URL_HOST);
                            $icon = $scheme . '://' . $domain . $href;
                        } else {
                            $icon = $base . '/' . ltrim($href, '/');
                        }
                    }
                }
            }
        }
    }

    return [
        'url' => $url,
        'title' => $title,
        'icon' => $icon,
    ];
}

function &find_drawer(array &$data, string $drawerId): array
{
    foreach ($data['drawers'] as &$drawer) {
        if (($drawer['id'] ?? '') === $drawerId) {
            return $drawer;
        }
    }

    $null = [];
    return $null;
}

function &find_site(array &$drawer, string $siteId): array
{
    foreach ($drawer['sites'] as &$site) {
        if (($site['id'] ?? '') === $siteId) {
            return $site;
        }
    }

    $null = [];
    return $null;
}

try {
    $method = $_SERVER['REQUEST_METHOD'] ?? 'GET';
    $path = parse_path();
    if ($path === '') {
        $path = '/';
    }

    if ($method === 'GET' && $path === '/nav') {
        send_json(200, read_data());
    }

    if ($method === 'POST' && $path === '/preview') {
        $body = read_json_body();
        $preview = fetch_preview((string) ($body['url'] ?? ''));
        send_json(200, $preview);
    }

    if ($method === 'POST' && $path === '/drawers') {
        $data = read_data();
        $body = read_json_body();
        $drawer = [
            'id' => make_id('drawer'),
            'name' => mb_substr((string) ($body['name'] ?? '新建文件夹'), 0, 30),
            'color' => preg_match('/^#[0-9a-fA-F]{6}$/', (string) ($body['color'] ?? '')) ? (string) $body['color'] : '#156f52',
            'sites' => [],
        ];
        $data['drawers'][] = $drawer;
        write_data($data);
        send_json(201, $drawer);
    }

    if (preg_match('#^/drawers/([^/]+)$#', $path, $m) === 1) {
        $drawerId = $m[1];
        $data = read_data();
        $drawer = &find_drawer($data, $drawerId);
        if ($drawer === []) {
            send_json(404, ['error' => 'Drawer not found']);
        }

        if ($method === 'PUT') {
            $body = read_json_body();
            $name = trim((string) ($body['name'] ?? ''));
            if ($name !== '') {
                $drawer['name'] = mb_substr($name, 0, 30);
            }
            $color = (string) ($body['color'] ?? '');
            if (preg_match('/^#[0-9a-fA-F]{6}$/', $color) === 1) {
                $drawer['color'] = $color;
            }
            write_data($data);
            send_json(200, $drawer);
        }

        if ($method === 'DELETE') {
            $data['drawers'] = array_values(array_filter($data['drawers'], static function ($d) use ($drawerId) {
                return ($d['id'] ?? '') !== $drawerId;
            }));
            write_data($data);
            send_json(200, ['ok' => true]);
        }
    }

    if (preg_match('#^/drawers/([^/]+)/sites$#', $path, $m) === 1 && $method === 'POST') {
        $drawerId = $m[1];
        $data = read_data();
        $drawer = &find_drawer($data, $drawerId);
        if ($drawer === []) {
            send_json(404, ['error' => 'Drawer not found']);
        }

        $body = read_json_body();
        $preview = fetch_preview((string) ($body['url'] ?? ''));
        $site = [
            'id' => make_id('site'),
            'url' => $preview['url'],
            'title' => $preview['title'],
            'icon' => $preview['icon'],
        ];
        $drawer['sites'][] = $site;
        write_data($data);
        send_json(201, $site);
    }

    if (preg_match('#^/drawers/([^/]+)/sites/([^/]+)$#', $path, $m) === 1) {
        $drawerId = $m[1];
        $siteId = $m[2];

        $data = read_data();
        $drawer = &find_drawer($data, $drawerId);
        if ($drawer === []) {
            send_json(404, ['error' => 'Drawer not found']);
        }

        $site = &find_site($drawer, $siteId);
        if ($site === []) {
            send_json(404, ['error' => 'Site not found']);
        }

        if ($method === 'PUT') {
            $body = read_json_body();
            $preview = fetch_preview((string) ($body['url'] ?? $site['url'] ?? ''));
            $site['url'] = $preview['url'];
            $site['title'] = $preview['title'];
            $site['icon'] = $preview['icon'];
            write_data($data);
            send_json(200, $site);
        }

        if ($method === 'DELETE') {
            $drawer['sites'] = array_values(array_filter($drawer['sites'], static function ($s) use ($siteId) {
                return ($s['id'] ?? '') !== $siteId;
            }));
            write_data($data);
            send_json(200, ['ok' => true]);
        }
    }

    send_json(404, ['error' => 'Not Found']);
} catch (Throwable $e) {
    send_json(400, ['error' => $e->getMessage()]);
}
