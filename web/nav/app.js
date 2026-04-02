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
const API_BASE = "./api/index.php";

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

editToggleEl.addEventListener("click", () => {
  state.editMode = !state.editMode;
  render();
});

addDrawerEl.addEventListener("click", addDrawer);
addSiteEl.addEventListener("click", addSite);
mobileToggleEl.addEventListener("click", () => {
  drawerPanelEl.classList.add("open");
  drawerMaskEl.classList.add("show");
});
drawerMaskEl.addEventListener("click", closeDrawerOnMobile);

loadData().catch((err) => {
  alert(err.message || "加载失败");
});
