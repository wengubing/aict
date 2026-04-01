# 网址导航主页

## 启动方式

在当前目录执行：

```bash
node server.js
```

默认访问地址：

- http://localhost:8787

## 功能

- 左侧抽屉(文件夹) + 右侧网址平铺大卡片
- 自动适配 PC 与手机
- 前端可新增/编辑/删除抽屉和网址
- 点击条目右上角 `✎` 可编辑抽屉信息或网址
- 网址在新增/编辑时自动抓取标题和图标(失败时回退域名和通用 favicon)
- 抽屉与网址数据持久化到 `nav-data.json`

## 后端接口

- `GET /api/nav`
- `POST /api/drawers`
- `PUT /api/drawers/:drawerId`
- `DELETE /api/drawers/:drawerId`
- `POST /api/drawers/:drawerId/sites`
- `PUT /api/drawers/:drawerId/sites/:siteId`
- `DELETE /api/drawers/:drawerId/sites/:siteId`
- `POST /api/preview`
