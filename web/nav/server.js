const http = require("http");
const fs = require("fs/promises");
const path = require("path");

const HOST = "0.0.0.0";
const PORT = process.env.PORT ? Number(process.env.PORT) : 8787;
const DATA_FILE = path.join(__dirname, "nav-data.json");

const MIME_TYPES = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".svg": "image/svg+xml",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".jpeg": "image/jpeg",
  ".ico": "image/x-icon"
};

function json(res, statusCode, payload) {
  res.writeHead(statusCode, { "Content-Type": "application/json; charset=utf-8" });
  res.end(JSON.stringify(payload, null, 2));
}

function notFound(res) {
  json(res, 404, { error: "Not Found" });
}

function makeId(prefix) {
  return `${prefix}_${Date.now().toString(36)}_${Math.random().toString(36).slice(2, 8)}`;
}

async function ensureDataFile() {
  try {
    await fs.access(DATA_FILE);
  } catch {
    const seed = {
      drawers: [
        {
          id: makeId("drawer"),
          name: "常用",
          color: "#146c94",
          sites: [
            {
              id: makeId("site"),
              url: "https://github.com",
              title: "GitHub",
              icon: "https://github.githubassets.com/favicons/favicon.svg"
            },
            {
              id: makeId("site"),
              url: "https://www.bing.com",
              title: "Bing",
              icon: "https://www.bing.com/sa/simg/favicon-trans-bg-blue-mg.ico"
            }
          ]
        }
      ]
    };
    await fs.writeFile(DATA_FILE, JSON.stringify(seed, null, 2), "utf8");
  }
}

async function readData() {
  await ensureDataFile();
  const raw = await fs.readFile(DATA_FILE, "utf8");
  const parsed = JSON.parse(raw);
  if (!parsed.drawers || !Array.isArray(parsed.drawers)) {
    return { drawers: [] };
  }
  return parsed;
}

async function writeData(data) {
  await fs.writeFile(DATA_FILE, JSON.stringify(data, null, 2), "utf8");
}

async function readBody(req) {
  return new Promise((resolve, reject) => {
    let body = "";
    req.on("data", (chunk) => {
      body += chunk;
      if (body.length > 1_000_000) {
        reject(new Error("Body too large"));
      }
    });
    req.on("end", () => {
      if (!body) {
        resolve({});
        return;
      }
      try {
        resolve(JSON.parse(body));
      } catch {
        reject(new Error("Invalid JSON body"));
      }
    });
    req.on("error", reject);
  });
}

function sanitizeUrl(url) {
  const u = String(url || "").trim();
  if (!u) {
    throw new Error("URL is required");
  }
  const withProto = /^https?:\/\//i.test(u) ? u : `https://${u}`;
  const finalUrl = new URL(withProto);
  if (!["http:", "https:"].includes(finalUrl.protocol)) {
    throw new Error("Only http/https URLs are supported");
  }
  return finalUrl.toString();
}

function fallbackIcon(url) {
  const host = new URL(url).hostname;
  return `https://www.google.com/s2/favicons?domain=${encodeURIComponent(host)}&sz=128`;
}

async function fetchPreview(inputUrl) {
  const url = sanitizeUrl(inputUrl);
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 5000);
  let title = new URL(url).hostname;
  let icon = fallbackIcon(url);

  try {
    const response = await fetch(url, {
      method: "GET",
      redirect: "follow",
      signal: controller.signal,
      headers: {
        "User-Agent": "Mozilla/5.0 (compatible; NavBot/1.0)"
      }
    });

    const html = await response.text();
    const titleMatch = html.match(/<title[^>]*>([\s\S]*?)<\/title>/i);
    if (titleMatch && titleMatch[1]) {
      title = titleMatch[1].trim().replace(/\s+/g, " ").slice(0, 80);
    }

    const iconMatch = html.match(/<link[^>]+rel=["'][^"']*icon[^"']*["'][^>]*>/i);
    if (iconMatch) {
      const hrefMatch = iconMatch[0].match(/href=["']([^"']+)["']/i);
      if (hrefMatch && hrefMatch[1]) {
        icon = new URL(hrefMatch[1], response.url || url).toString();
      }
    }
  } catch {
    // fall back to hostname and generic favicon
  } finally {
    clearTimeout(timeout);
  }

  return { url, title, icon };
}

function findDrawer(data, drawerId) {
  return data.drawers.find((d) => d.id === drawerId);
}

function findSite(drawer, siteId) {
  return drawer.sites.find((s) => s.id === siteId);
}

async function handleApi(req, res, pathname) {
  try {
    if (req.method === "GET" && pathname === "/api/nav") {
      const data = await readData();
      json(res, 200, data);
      return true;
    }

    if (req.method === "POST" && pathname === "/api/preview") {
      const body = await readBody(req);
      const preview = await fetchPreview(body.url);
      json(res, 200, preview);
      return true;
    }

    if (req.method === "POST" && pathname === "/api/drawers") {
      const data = await readData();
      const body = await readBody(req);
      const drawer = {
        id: makeId("drawer"),
        name: String(body.name || "新建文件夹").slice(0, 30),
        color: /^#[0-9a-fA-F]{6}$/.test(body.color || "") ? body.color : "#156f52",
        sites: []
      };
      data.drawers.push(drawer);
      await writeData(data);
      json(res, 201, drawer);
      return true;
    }

    const drawerMatch = pathname.match(/^\/api\/drawers\/([^/]+)$/);
    if (drawerMatch) {
      const drawerId = drawerMatch[1];
      const data = await readData();
      const drawer = findDrawer(data, drawerId);
      if (!drawer) {
        notFound(res);
        return true;
      }

      if (req.method === "PUT") {
        const body = await readBody(req);
        drawer.name = String(body.name || drawer.name).slice(0, 30);
        if (/^#[0-9a-fA-F]{6}$/.test(body.color || "")) {
          drawer.color = body.color;
        }
        await writeData(data);
        json(res, 200, drawer);
        return true;
      }

      if (req.method === "DELETE") {
        data.drawers = data.drawers.filter((d) => d.id !== drawerId);
        await writeData(data);
        json(res, 200, { ok: true });
        return true;
      }
    }

    const siteCreateMatch = pathname.match(/^\/api\/drawers\/([^/]+)\/sites$/);
    if (siteCreateMatch && req.method === "POST") {
      const drawerId = siteCreateMatch[1];
      const data = await readData();
      const drawer = findDrawer(data, drawerId);
      if (!drawer) {
        notFound(res);
        return true;
      }

      const body = await readBody(req);
      const preview = await fetchPreview(body.url);
      const site = {
        id: makeId("site"),
        url: preview.url,
        title: preview.title,
        icon: preview.icon
      };
      drawer.sites.push(site);
      await writeData(data);
      json(res, 201, site);
      return true;
    }

    const siteMatch = pathname.match(/^\/api\/drawers\/([^/]+)\/sites\/([^/]+)$/);
    if (siteMatch) {
      const drawerId = siteMatch[1];
      const siteId = siteMatch[2];
      const data = await readData();
      const drawer = findDrawer(data, drawerId);
      if (!drawer) {
        notFound(res);
        return true;
      }

      const site = findSite(drawer, siteId);
      if (!site) {
        notFound(res);
        return true;
      }

      if (req.method === "PUT") {
        const body = await readBody(req);
        const preview = await fetchPreview(body.url || site.url);
        site.url = preview.url;
        site.title = preview.title;
        site.icon = preview.icon;
        await writeData(data);
        json(res, 200, site);
        return true;
      }

      if (req.method === "DELETE") {
        drawer.sites = drawer.sites.filter((s) => s.id !== siteId);
        await writeData(data);
        json(res, 200, { ok: true });
        return true;
      }
    }

    return false;
  } catch (error) {
    json(res, 400, { error: error.message || "Bad request" });
    return true;
  }
}

async function serveStatic(req, res, pathname) {
  const normalized = pathname === "/" ? "/index.html" : pathname;
  const filePath = path.join(__dirname, normalized);

  if (!filePath.startsWith(__dirname)) {
    notFound(res);
    return;
  }

  try {
    const content = await fs.readFile(filePath);
    const ext = path.extname(filePath).toLowerCase();
    res.writeHead(200, { "Content-Type": MIME_TYPES[ext] || "application/octet-stream" });
    res.end(content);
  } catch {
    notFound(res);
  }
}

const server = http.createServer(async (req, res) => {
  const urlObj = new URL(req.url, `http://${req.headers.host}`);
  const pathname = decodeURIComponent(urlObj.pathname);

  if (pathname.startsWith("/api/")) {
    const handled = await handleApi(req, res, pathname);
    if (!handled) {
      notFound(res);
    }
    return;
  }

  await serveStatic(req, res, pathname);
});

server.listen(PORT, HOST, () => {
  console.log(`导航站已启动: http://localhost:${PORT}`);
});
