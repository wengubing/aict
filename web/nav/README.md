# 网址导航主页

## 运行方式（Nginx + PHP）

将整个 `web/nav` 目录复制到支持 PHP 的站点目录即可。

前端请求路径已固定为：

- `./api/index.php/*`

只要 PHP 可执行，页面就能直接工作。

## 本地快速调试（可选）

在当前目录执行：

```bash
php -S 127.0.0.1:8787
```

然后访问：

- http://127.0.0.1:8787

## 功能

- 左侧抽屉(文件夹) + 右侧网址平铺大卡片
- 自动适配 PC 与手机
- 打开页面先进行口令验证，已登录状态自动保持 24 小时
- 前端可新增/编辑/删除抽屉和网址
- 点击条目右上角 `✎` 可编辑抽屉信息或网址
- 网址在新增/编辑时自动抓取标题和图标(失败时回退域名和通用 favicon)
- 抽屉与网址数据持久化到 `nav-data.json`（PHP 读写）

## 登录口令配置

- 在 `index.html` 的 `body` 标签设置 `data-passcode`，例如：`data-passcode="123456"`
- 浏览器本地会记录登录有效期 24 小时，到期后会自动要求重新输入口令
- 页面右上角提供“退出登录”按钮，可随时手动退出

## 后端接口（PHP）

- `GET ./api/index.php/nav`
- `POST ./api/index.php/drawers`
- `PUT ./api/index.php/drawers/:drawerId`
- `DELETE ./api/index.php/drawers/:drawerId`
- `POST ./api/index.php/drawers/:drawerId/sites`
- `PUT ./api/index.php/drawers/:drawerId/sites/:siteId`
- `DELETE ./api/index.php/drawers/:drawerId/sites/:siteId`
- `POST ./api/index.php/preview`

## Nginx 权限提示

请确保 `nav-data.json` 对 PHP 进程用户可写，否则新增/编辑/删除会失败。
