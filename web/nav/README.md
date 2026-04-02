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
- 提供 JSON 备份下载与本地文件恢复
- 支持坚果云 WebDAV 手动备份到云、从云恢复
- 支持前端自动定时备份到云、自动定时从云恢复
- 点击条目右上角 `✎` 可编辑抽屉信息或网址
- 网址在新增/编辑时自动抓取标题和图标(失败时回退域名和通用 favicon)
- 抽屉与网址数据持久化到 `nav-data.json`（PHP 读写）

## 登录口令配置

- 在 `index.html` 的 `body` 标签设置 `data-passcode`，例如：`data-passcode="123456"`
- 浏览器本地会记录登录有效期 24 小时，到期后会自动要求重新输入口令
- 页面右上角提供“退出登录”按钮，可随时手动退出

## 后端接口（PHP）

- `GET ./api/index.php/nav`
- `GET ./api/index.php/backup`
- `POST ./api/index.php/restore`
- `POST ./api/index.php/webdav/backup`
- `POST ./api/index.php/webdav/restore`
- `POST ./api/index.php/drawers`
- `PUT ./api/index.php/drawers/:drawerId`
- `DELETE ./api/index.php/drawers/:drawerId`
- `POST ./api/index.php/drawers/:drawerId/sites`
- `PUT ./api/index.php/drawers/:drawerId/sites/:siteId`
- `DELETE ./api/index.php/drawers/:drawerId/sites/:siteId`
- `POST ./api/index.php/preview`

## 备份恢复使用

- 页面点击“备份JSON”会下载当前数据快照文件
- 页面点击“恢复JSON”可选择本地备份文件并覆盖恢复
- 建议先备份再恢复，避免误覆盖

## 坚果云 WebDAV 使用

- 页面点击“云配置”，填写 WebDAV 地址、用户名、应用密码、云端文件路径
- 坚果云推荐地址：`https://dav.jianguoyun.com/dav`
- 点击“备份到云”可手动上传当前 `nav-data.json`
- 点击“从云恢复”可拉取云端备份并覆盖本地数据
- 云配置里可启用自动定时备份/恢复（按分钟间隔）

注意：
- 自动定时任务由前端页面触发，页面打开且已登录时才会执行
- 使用 WebDAV 需 PHP 启用 `cURL` 扩展

## Nginx 权限提示

请确保 `nav-data.json` 对 PHP 进程用户可写，否则新增/编辑/删除会失败。
