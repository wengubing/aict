const state = {
  data: { drawers: [] },
  activeDrawerId: null,
  editMode: false
};

const drawerListEl = document.querySelector("#drawerList");
const siteGridEl = document.querySelector("#siteGrid");
const drawerTitleEl = document.querySelector("#drawerTitle");
const editToggleEl = document.querySelector("#editToggle");
const addDrawerEl = document.querySelector("#addDrawer");
const addSiteEl = document.querySelector("#addSite");
const drawerPanelEl = document.querySelector("#drawerPanel");
const drawerMaskEl = document.querySelector("#drawerMask");
const mobileToggleEl = document.querySelector("#mobileToggle");
const authScreenEl = document.querySelector("#authScreen");
const authFormEl = document.querySelector("#authForm");
const authInputEl = document.querySelector("#authInput");
const authErrorEl = document.querySelector("#authError");
const backupBtnEl = document.querySelector("#backupBtn");
const restoreBtnEl = document.querySelector("#restoreBtn");
const restoreFileInputEl = document.querySelector("#restoreFileInput");
const cloudConfigBtnEl = document.querySelector("#cloudConfigBtn");
const cloudBackupBtnEl = document.querySelector("#cloudBackupBtn");
const cloudRestoreBtnEl = document.querySelector("#cloudRestoreBtn");
const cloudConfigModalEl = document.querySelector("#cloudConfigModal");
const cloudConfigFormEl = document.querySelector("#cloudConfigForm");
const cloudConfigCancelEl = document.querySelector("#cloudConfigCancel");
const cloudBaseUrlEl = document.querySelector("#cloudBaseUrl");
const cloudUsernameEl = document.querySelector("#cloudUsername");
const cloudPasswordEl = document.querySelector("#cloudPassword");
const cloudRemoteFileEl = document.querySelector("#cloudRemoteFile");
const autoBackupEnabledEl = document.querySelector("#autoBackupEnabled");
const autoBackupMinutesEl = document.querySelector("#autoBackupMinutes");
const autoRestoreEnabledEl = document.querySelector("#autoRestoreEnabled");
const autoRestoreMinutesEl = document.querySelector("#autoRestoreMinutes");
const logoutBtnEl = document.querySelector("#logoutBtn");
const API_BASE = "./api/index.php";
const PASSCODE = document.body.dataset.passcode || "";
const AUTH_KEY = "nav_auth_until";
const AUTH_TTL_MS = 24 * 60 * 60 * 1000;

let initialized = false;
let autoBackupTimer = null;
let autoRestoreTimer = null;
let cloudBusy = false;
let cloudConfig = null;

async function api(path, options = {}) {
  const res = await fetch(`${API_BASE}${path}`, {
    headers: { "Content-Type": "application/json" },
    ...options
  });
  if (!res.ok) {
    const err = await res.json().catch(() => ({ error: "请求失败" }));
    throw new Error(err.error || "请求失败");
  }
  return res.json();
}

function isAuthenticated() {
  const raw = localStorage.getItem(AUTH_KEY);
  if (!raw) {
    return false;
  }
  const expiresAt = Number(raw);
  return Number.isFinite(expiresAt) && expiresAt > Date.now();
}

function setAuthenticated() {
  localStorage.setItem(AUTH_KEY, String(Date.now() + AUTH_TTL_MS));
}

function clearAuthenticated() {
  localStorage.removeItem(AUTH_KEY);
}

function getDefaultCloudConfig() {
  return {
    baseUrl: "https://dav.jianguoyun.com/dav",
    username: "",
    password: "",
    remoteFile: "nav-backup/nav-data.json",
    autoBackupEnabled: false,
    autoBackupMinutes: 60,
    autoRestoreEnabled: false,
    autoRestoreMinutes: 120
  };
}

function getCloudConfig() {
  return cloudConfig || getDefaultCloudConfig();
}

function hasCloudCredentials(config) {
  return Boolean(config.baseUrl && config.username && config.password && config.remoteFile);
}

function fillCloudConfigForm() {
  const current = getCloudConfig();
  cloudBaseUrlEl.value = current.baseUrl;
  cloudUsernameEl.value = current.username;
  cloudPasswordEl.value = current.password;
  cloudRemoteFileEl.value = current.remoteFile;
  autoBackupEnabledEl.checked = current.autoBackupEnabled;
  autoBackupMinutesEl.value = String(current.autoBackupMinutes);
  autoRestoreEnabledEl.checked = current.autoRestoreEnabled;
  autoRestoreMinutesEl.value = String(current.autoRestoreMinutes);
}

function readCloudConfigForm() {
  return {
    baseUrl: cloudBaseUrlEl.value.trim(),
    username: cloudUsernameEl.value.trim(),
    password: cloudPasswordEl.value.trim(),
    remoteFile: cloudRemoteFileEl.value.trim(),
    autoBackupEnabled: autoBackupEnabledEl.checked,
    autoBackupMinutes: Math.max(5, Number(autoBackupMinutesEl.value) || 60),
    autoRestoreEnabled: autoRestoreEnabledEl.checked,
    autoRestoreMinutes: Math.max(5, Number(autoRestoreMinutesEl.value) || 120)
  };
}

function openCloudConfigModal() {
  fillCloudConfigForm();
  cloudConfigModalEl.classList.remove("hidden");
}

function closeCloudConfigModal() {
  cloudConfigModalEl.classList.add("hidden");
}

async function cloudBackup(silent = false) {
  const config = getCloudConfig();
  if (!hasCloudCredentials(config)) {
    throw new Error("请先完成云配置");
  }

  if (cloudBusy) {
    return;
  }
  cloudBusy = true;
  try {
    const result = await api("/webdav/backup", { method: "POST" });
    if (!silent) {
      const tip = result?.remoteFile ? `已备份到云端: ${result.remoteFile}` : "已备份到云端";
      alert(tip);
    }
  } finally {
    cloudBusy = false;
  }
}

async function cloudRestore(silent = false) {
  const config = getCloudConfig();
  if (!hasCloudCredentials(config)) {
    throw new Error("请先完成云配置");
  }

  if (cloudBusy) {
    return;
  }
  cloudBusy = true;
  try {
    await api("/webdav/restore", { method: "POST" });
    await loadData();
    await loadCloudConfig();
    setupAutoJobs();
    if (!silent) {
      alert("已从云端恢复");
    }
  } finally {
    cloudBusy = false;
  }
}

function clearAutoJobs() {
  if (autoBackupTimer) {
    clearInterval(autoBackupTimer);
    autoBackupTimer = null;
  }
  if (autoRestoreTimer) {
    clearInterval(autoRestoreTimer);
    autoRestoreTimer = null;
  }
}

function setupAutoJobs() {
  clearAutoJobs();
  const config = getCloudConfig();
  if (!hasCloudCredentials(config)) {
    return;
  }

  if (config.autoBackupEnabled) {
    autoBackupTimer = setInterval(() => {
      cloudBackup(true).catch((err) => {
        console.warn("自动云备份失败", err);
      });
    }, config.autoBackupMinutes * 60 * 1000);
  }

  if (config.autoRestoreEnabled) {
    autoRestoreTimer = setInterval(() => {
      cloudRestore(true).catch((err) => {
        console.warn("自动云恢复失败", err);
      });
    }, config.autoRestoreMinutes * 60 * 1000);
  }
}

async function loadCloudConfig() {
  const cfg = await api("/config");
  cloudConfig = cfg?.webdav ? cfg.webdav : getDefaultCloudConfig();
}

async function persistCloudConfig(config) {
  const saved = await api("/config", {
    method: "POST",
    body: JSON.stringify({ webdav: config })
  });
  cloudConfig = saved?.webdav ? saved.webdav : config;
}

function lockApp() {
  document.body.classList.add("auth-locked");
  authScreenEl.classList.remove("hidden");
  authInputEl.value = "";
  authErrorEl.textContent = "";
}

function unlockApp() {
  document.body.classList.remove("auth-locked");
  authScreenEl.classList.add("hidden");
}

async function startApp() {
  if (initialized) {
    return;
  }
  initialized = true;
  await loadData();
  await loadCloudConfig();
  setupAutoJobs();
}

async function verifyAndStart() {
  if (PASSCODE === "") {
    authErrorEl.textContent = "未配置口令，请在 HTML 的 data-passcode 中设置。";
    lockApp();
    return;
  }

  if (isAuthenticated()) {
    unlockApp();
    await startApp();
    return;
  }

  lockApp();
  authInputEl.focus();
}

function getActiveDrawer() {
  return state.data.drawers.find((d) => d.id === state.activeDrawerId) || state.data.drawers[0] || null;
}

function closeDrawerOnMobile() {
  drawerPanelEl.classList.remove("open");
  drawerMaskEl.classList.remove("show");
}

function renderDrawers() {
  drawerListEl.innerHTML = "";

  state.data.drawers.forEach((drawer) => {
    const btn = document.createElement("button");
    btn.className = `drawer-item ${drawer.id === state.activeDrawerId ? "active" : ""}`;
    btn.style.background = drawer.color;
    btn.innerHTML = `<strong>${escapeHtml(drawer.name)}</strong><small>${drawer.sites.length} 个站点</small>`;
    btn.addEventListener("click", () => {
      state.activeDrawerId = drawer.id;
      render();
      closeDrawerOnMobile();
    });

    const editBtn = document.createElement("button");
    editBtn.className = "marker-btn";
    editBtn.textContent = "✎";
    editBtn.title = "编辑抽屉";
    editBtn.addEventListener("click", (ev) => {
      ev.stopPropagation();
      editDrawer(drawer);
    });

    btn.appendChild(editBtn);
    drawerListEl.appendChild(btn);
  });

  const addBtn = document.createElement("button");
  addBtn.className = "add-card";
  addBtn.textContent = "+ 新建抽屉";
  addBtn.addEventListener("click", addDrawer);
  drawerListEl.appendChild(addBtn);
}

function renderSites() {
  siteGridEl.innerHTML = "";
  const drawer = getActiveDrawer();
  if (!drawer) {
    drawerTitleEl.textContent = "请先创建抽屉";
    return;
  }

  drawerTitleEl.textContent = drawer.name;

  drawer.sites.forEach((site) => {
    const card = document.createElement("a");
    card.className = "site-card";
    card.href = site.url;
    card.target = "_blank";
    card.rel = "noreferrer noopener";

    if (state.editMode) {
      card.addEventListener("click", (e) => e.preventDefault());
    }

    card.innerHTML = `
      <div class="thumb"><img src="${escapeAttr(site.icon)}" alt="icon" loading="lazy"></div>
      <div class="site-body">
        <p class="site-title">${escapeHtml(site.title)}</p>
        <p class="site-url">${escapeHtml(site.url)}</p>
      </div>
      <div class="site-actions">
        <button title="编辑网址" data-act="edit">✎</button>
        <button class="delete-btn" title="删除" data-act="delete">🗑</button>
      </div>
    `;

    card.querySelector('[data-act="edit"]').addEventListener("click", async (ev) => {
      ev.preventDefault();
      const nextUrl = prompt("输入网址", site.url);
      if (!nextUrl) {
        return;
      }
      await api(`/drawers/${drawer.id}/sites/${site.id}`, {
        method: "PUT",
        body: JSON.stringify({ url: nextUrl })
      });
      await loadData();
    });

    card.querySelector('[data-act="delete"]').addEventListener("click", async (ev) => {
      ev.preventDefault();
      if (!confirm("确认删除该网址吗？")) {
        return;
      }
      await api(`/drawers/${drawer.id}/sites/${site.id}`, { method: "DELETE" });
      await loadData();
    });

    siteGridEl.appendChild(card);
  });

  const addCard = document.createElement("button");
  addCard.className = "add-card";
  addCard.textContent = "+ 新增网址";
  addCard.addEventListener("click", addSite);
  siteGridEl.appendChild(addCard);
}

function render() {
  document.body.classList.toggle("edit-mode", state.editMode);
  editToggleEl.textContent = state.editMode ? "退出编辑" : "编辑模式";
  renderDrawers();
  renderSites();
}

async function addDrawer() {
  const name = prompt("抽屉名称", "新建文件夹");
  if (!name) {
    return;
  }
  const color = prompt("背景颜色 (HEX, 例如 #156f52)", "#156f52") || "#156f52";
  const created = await api("/drawers", {
    method: "POST",
    body: JSON.stringify({ name, color })
  });
  state.activeDrawerId = created.id;
  await loadData();
}

async function editDrawer(drawer) {
  const name = prompt("抽屉名称", drawer.name);
  if (!name) {
    return;
  }
  const color = prompt("背景颜色 (HEX)", drawer.color) || drawer.color;
  await api(`/drawers/${drawer.id}`, {
    method: "PUT",
    body: JSON.stringify({ name, color })
  });

  if (state.editMode && confirm("是否删除该抽屉？")) {
    await api(`/drawers/${drawer.id}`, { method: "DELETE" });
    if (state.activeDrawerId === drawer.id) {
      state.activeDrawerId = state.data.drawers[0]?.id || null;
    }
  }

  await loadData();
}

async function addSite() {
  const drawer = getActiveDrawer();
  if (!drawer) {
    alert("请先创建抽屉");
    return;
  }
  const url = prompt("输入网址", "https://");
  if (!url || url === "https://") {
    return;
  }
  await api(`/drawers/${drawer.id}/sites`, {
    method: "POST",
    body: JSON.stringify({ url })
  });
  await loadData();
}

async function loadData() {
  state.data = await api("/nav");
  if (!state.activeDrawerId && state.data.drawers.length) {
    state.activeDrawerId = state.data.drawers[0].id;
  }

  if (state.activeDrawerId && !state.data.drawers.some((d) => d.id === state.activeDrawerId)) {
    state.activeDrawerId = state.data.drawers[0]?.id || null;
  }

  render();
}

function escapeHtml(input) {
  return String(input)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function escapeAttr(input) {
  return escapeHtml(input).replaceAll("`", "");
}

function triggerBackup() {
  const ts = Date.now();
  window.location.href = `${API_BASE}/backup?t=${ts}`;
}

async function restoreFromFile(file) {
  const text = await file.text();
  let parsed;
  try {
    parsed = JSON.parse(text);
  } catch {
    throw new Error("JSON 文件格式无效");
  }

  if (!parsed || !Array.isArray(parsed.drawers)) {
    throw new Error("恢复文件缺少 drawers 数组");
  }

  await api("/restore", {
    method: "POST",
    body: JSON.stringify(parsed)
  });
  await loadData();
  await loadCloudConfig();
  setupAutoJobs();
}

editToggleEl.addEventListener("click", () => {
  state.editMode = !state.editMode;
  render();
});

addDrawerEl.addEventListener("click", addDrawer);
addSiteEl.addEventListener("click", addSite);
backupBtnEl.addEventListener("click", triggerBackup);
restoreBtnEl.addEventListener("click", () => {
  restoreFileInputEl.value = "";
  restoreFileInputEl.click();
});
cloudConfigBtnEl.addEventListener("click", () => {
  openCloudConfigModal();
});
cloudConfigCancelEl.addEventListener("click", closeCloudConfigModal);
cloudConfigModalEl.addEventListener("click", (ev) => {
  if (ev.target === cloudConfigModalEl) {
    closeCloudConfigModal();
  }
});
cloudConfigFormEl.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  const config = readCloudConfigForm();
  if (!hasCloudCredentials(config)) {
    alert("请完整填写 WebDAV 地址、用户名、密码和云端文件路径");
    return;
  }

  try {
    await persistCloudConfig(config);
    setupAutoJobs();
    closeCloudConfigModal();
    alert("云配置已保存到 nav-data.json");
  } catch (err) {
    alert(err.message || "保存云配置失败");
  }
});
cloudBackupBtnEl.addEventListener("click", async () => {
  try {
    await cloudBackup(false);
  } catch (err) {
    alert(err.message || "云备份失败");
  }
});
cloudRestoreBtnEl.addEventListener("click", async () => {
  if (!confirm("从云端恢复会覆盖当前全部数据，确定继续吗？")) {
    return;
  }

  try {
    await cloudRestore(false);
  } catch (err) {
    alert(err.message || "云恢复失败");
  }
});
restoreFileInputEl.addEventListener("change", async () => {
  const file = restoreFileInputEl.files?.[0];
  if (!file) {
    return;
  }

  if (!confirm("恢复将覆盖当前全部抽屉和网址，确定继续吗？")) {
    return;
  }

  try {
    await restoreFromFile(file);
    alert("恢复成功");
  } catch (err) {
    alert(err.message || "恢复失败");
  }
});
mobileToggleEl.addEventListener("click", () => {
  drawerPanelEl.classList.add("open");
  drawerMaskEl.classList.add("show");
});
drawerMaskEl.addEventListener("click", closeDrawerOnMobile);
authFormEl.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  if (authInputEl.value !== PASSCODE) {
    authErrorEl.textContent = "口令错误，请重试。";
    authInputEl.focus();
    authInputEl.select();
    return;
  }

  setAuthenticated();
  unlockApp();
  try {
    await startApp();
  } catch (err) {
    alert(err.message || "加载失败");
    lockApp();
  }
});

logoutBtnEl.addEventListener("click", () => {
  clearAuthenticated();
  clearAutoJobs();
  lockApp();
});

verifyAndStart().catch((err) => {
  alert(err.message || "加载失败");
  lockApp();
});
