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

function send_download_json(array $payload, string $filename): void
{
    header_remove('Content-Type');
    header('Content-Type: application/json; charset=utf-8');
    header('Content-Disposition: attachment; filename="' . $filename . '"');
    http_response_code(200);
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
    $phpPos = strpos($uriPath, '.php');
    if ($phpPos !== false) {
        $afterPhp = substr($uriPath, $phpPos + 4);
        if ($afterPhp !== false && $afterPhp !== '') {
            return $afterPhp;
        }
    }

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

function default_webdav_config(): array
{
    return [
        'baseUrl' => 'https://dav.jianguoyun.com/dav',
        'username' => '',
        'password' => '',
        'remoteFile' => 'nav-backup/nav-data.json',
        'autoBackupEnabled' => false,
        'autoBackupMinutes' => 60,
        'autoRestoreEnabled' => false,
        'autoRestoreMinutes' => 120,
    ];
}

function default_config(): array
{
    return [
        'webdav' => default_webdav_config(),
    ];
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
        'config' => default_config(),
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
        return ['drawers' => [], 'config' => default_config()];
    }

    if (!isset($parsed['config']) || !is_array($parsed['config'])) {
        $parsed['config'] = default_config();
    }

    $parsed['config']['webdav'] = normalize_webdav_config_settings((array) ($parsed['config']['webdav'] ?? []));

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

function normalize_webdav_config_settings(array $input): array
{
    $defaults = default_webdav_config();
    return [
        'baseUrl' => rtrim(trim((string) ($input['baseUrl'] ?? $defaults['baseUrl'])), '/'),
        'username' => trim((string) ($input['username'] ?? $defaults['username'])),
        'password' => trim((string) ($input['password'] ?? $defaults['password'])),
        'remoteFile' => ltrim(trim((string) ($input['remoteFile'] ?? $defaults['remoteFile'])), '/'),
        'autoBackupEnabled' => (bool) ($input['autoBackupEnabled'] ?? $defaults['autoBackupEnabled']),
        'autoBackupMinutes' => max(5, (int) ($input['autoBackupMinutes'] ?? $defaults['autoBackupMinutes'])),
        'autoRestoreEnabled' => (bool) ($input['autoRestoreEnabled'] ?? $defaults['autoRestoreEnabled']),
        'autoRestoreMinutes' => max(5, (int) ($input['autoRestoreMinutes'] ?? $defaults['autoRestoreMinutes'])),
    ];
}

function normalize_data(array $raw, ?array $fallbackConfig = null): array
{
    if (!isset($raw['drawers']) || !is_array($raw['drawers'])) {
        throw new InvalidArgumentException('恢复数据格式错误：缺少 drawers 数组');
    }

    $result = [
        'drawers' => [],
        'config' => is_array($fallbackConfig) ? $fallbackConfig : default_config(),
    ];

    if (isset($raw['config']) && is_array($raw['config'])) {
        $result['config'] = [
            'webdav' => normalize_webdav_config_settings((array) ($raw['config']['webdav'] ?? [])),
        ];
    }
    foreach ($raw['drawers'] as $drawer) {
        if (!is_array($drawer)) {
            continue;
        }

        $drawerId = trim((string) ($drawer['id'] ?? ''));
        $drawerName = trim((string) ($drawer['name'] ?? ''));
        $drawerColor = (string) ($drawer['color'] ?? '#156f52');
        $sites = isset($drawer['sites']) && is_array($drawer['sites']) ? $drawer['sites'] : [];

        if ($drawerId === '' || $drawerName === '') {
            continue;
        }

        if (preg_match('/^#[0-9a-fA-F]{6}$/', $drawerColor) !== 1) {
            $drawerColor = '#156f52';
        }

        $normalizedSites = [];
        foreach ($sites as $site) {
            if (!is_array($site)) {
                continue;
            }

            $siteId = trim((string) ($site['id'] ?? ''));
            $siteUrlRaw = trim((string) ($site['url'] ?? ''));
            if ($siteId === '' || $siteUrlRaw === '') {
                continue;
            }

            $siteUrl = sanitize_url($siteUrlRaw);
            $siteTitle = trim((string) ($site['title'] ?? ''));
            if ($siteTitle === '') {
                $siteTitle = (string) (parse_url($siteUrl, PHP_URL_HOST) ?: $siteUrl);
            }

            $siteIcon = trim((string) ($site['icon'] ?? ''));
            if ($siteIcon === '') {
                $siteIcon = fallback_icon($siteUrl);
            }

            $normalizedSites[] = [
                'id' => $siteId,
                'url' => $siteUrl,
                'title' => mb_substr($siteTitle, 0, 80),
                'icon' => $siteIcon,
            ];
        }

        $result['drawers'][] = [
            'id' => $drawerId,
            'name' => mb_substr($drawerName, 0, 30),
            'color' => $drawerColor,
            'sites' => $normalizedSites,
        ];
    }

    return $result;
}

function normalize_webdav_config(array $body): array
{
    $baseUrl = rtrim(trim((string) ($body['baseUrl'] ?? '')), '/');
    $username = trim((string) ($body['username'] ?? ''));
    $password = trim((string) ($body['password'] ?? ''));
    $remoteFile = ltrim(trim((string) ($body['remoteFile'] ?? '')), '/');

    if ($baseUrl === '' || $username === '' || $password === '' || $remoteFile === '') {
        throw new InvalidArgumentException('WebDAV 配置不完整');
    }

    if (preg_match('/^https?:\/\//i', $baseUrl) !== 1) {
        throw new InvalidArgumentException('WebDAV 地址必须是 http/https');
    }

    return [
        'baseUrl' => $baseUrl,
        'username' => $username,
        'password' => $password,
        'remoteFile' => $remoteFile,
    ];
}

function require_webdav_credentials(array $cfg): array
{
    $cfg = normalize_webdav_config($cfg);
    return $cfg;
}

function webdav_request(string $method, string $url, string $username, string $password, ?string $body = null): array
{
    $ch = curl_init($url);
    if ($ch === false) {
        throw new RuntimeException('初始化 WebDAV 连接失败');
    }

    $headers = [];
    if ($body !== null) {
        $headers[] = 'Content-Type: application/json; charset=utf-8';
        curl_setopt($ch, CURLOPT_POSTFIELDS, $body);
    }

    curl_setopt_array($ch, [
        CURLOPT_CUSTOMREQUEST => $method,
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_FOLLOWLOCATION => true,
        CURLOPT_TIMEOUT => 20,
        CURLOPT_USERPWD => $username . ':' . $password,
        CURLOPT_HTTPAUTH => CURLAUTH_BASIC,
        CURLOPT_HTTPHEADER => $headers,
    ]);

    $responseBody = curl_exec($ch);
    $err = curl_error($ch);
    $status = (int) curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);

    if ($responseBody === false) {
        throw new RuntimeException('WebDAV 请求失败: ' . $err);
    }

    return ['status' => $status, 'body' => (string) $responseBody];
}

function build_webdav_url(string $baseUrl, string $remotePath): string
{
    return $baseUrl . '/' . str_replace('%2F', '/', rawurlencode($remotePath));
}

function ensure_webdav_dirs(string $baseUrl, string $username, string $password, string $remoteFile): void
{
    $parts = explode('/', $remoteFile);
    array_pop($parts);
    if ($parts === []) {
        return;
    }

    $current = '';
    foreach ($parts as $segment) {
        $segment = trim($segment);
        if ($segment === '') {
            continue;
        }

        $current = $current === '' ? $segment : ($current . '/' . $segment);
        $url = build_webdav_url($baseUrl, $current);
        $res = webdav_request('MKCOL', $url, $username, $password, null);
        if (!in_array($res['status'], [201, 405, 301], true)) {
            throw new RuntimeException('创建云目录失败，HTTP ' . $res['status']);
        }
    }
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

    if ($method === 'GET' && $path === '/config') {
        $data = read_data();
        send_json(200, $data['config']);
    }

    if (($method === 'PUT' || $method === 'POST') && $path === '/config') {
        $data = read_data();
        $body = read_json_body();
        $webdavInput = isset($body['webdav']) && is_array($body['webdav']) ? $body['webdav'] : [];
        $data['config']['webdav'] = normalize_webdav_config_settings($webdavInput);
        write_data($data);
        send_json(200, $data['config']);
    }

    if ($method === 'GET' && $path === '/backup') {
        $data = read_data();
        $filename = 'nav-backup-' . date('Ymd-His') . '.json';
        send_download_json($data, $filename);
    }

    if ($method === 'POST' && $path === '/restore') {
        $current = read_data();
        $body = read_json_body();
        $normalized = normalize_data($body, $current['config'] ?? default_config());
        write_data($normalized);
        send_json(200, ['ok' => true, 'drawers' => count($normalized['drawers'])]);
    }

    if ($method === 'POST' && $path === '/webdav/backup') {
        if (!function_exists('curl_init')) {
            throw new RuntimeException('PHP 未启用 cURL 扩展，无法使用 WebDAV');
        }

        $body = read_json_body();
        if ($body !== []) {
            $cfg = require_webdav_credentials($body);
        } else {
            $cfg = require_webdav_credentials((array) (read_data()['config']['webdav'] ?? []));
        }
        ensure_webdav_dirs($cfg['baseUrl'], $cfg['username'], $cfg['password'], $cfg['remoteFile']);

        $json = json_encode(read_data(), JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT);
        if ($json === false) {
            throw new RuntimeException('本地数据编码失败');
        }

        $url = build_webdav_url($cfg['baseUrl'], $cfg['remoteFile']);
        $res = webdav_request('PUT', $url, $cfg['username'], $cfg['password'], $json);
        if (!in_array($res['status'], [200, 201, 204], true)) {
            throw new RuntimeException('云备份失败，HTTP ' . $res['status']);
        }

        send_json(200, ['ok' => true]);
    }

    if ($method === 'POST' && $path === '/webdav/restore') {
        if (!function_exists('curl_init')) {
            throw new RuntimeException('PHP 未启用 cURL 扩展，无法使用 WebDAV');
        }

        $body = read_json_body();
        if ($body !== []) {
            $cfg = require_webdav_credentials($body);
        } else {
            $cfg = require_webdav_credentials((array) (read_data()['config']['webdav'] ?? []));
        }
        $url = build_webdav_url($cfg['baseUrl'], $cfg['remoteFile']);
        $res = webdav_request('GET', $url, $cfg['username'], $cfg['password'], null);
        if ($res['status'] !== 200) {
            throw new RuntimeException('云恢复失败，HTTP ' . $res['status']);
        }

        $decoded = json_decode($res['body'], true);
        if (!is_array($decoded)) {
            throw new RuntimeException('云端备份文件不是有效 JSON');
        }

        $normalized = normalize_data($decoded);
        write_data($normalized);
        send_json(200, ['ok' => true, 'drawers' => count($normalized['drawers'])]);
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
