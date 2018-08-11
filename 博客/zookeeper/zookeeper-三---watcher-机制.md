## watcher 机制

1. 针对每一个节点的操作，zk 都会有一个监督者 watcher
2. 当监控的某个 znode 发生变化，会触发 watcher 事件
3. watcher 是一次性的，触发后立即销毁
4. 父，子节点的增删改都能触发其 watcher
5. 不同操作，触发的 watcher 事件不同

get path [watch]
NodeDataChanged、NodeDeleted

stat path [watch]
NodeCreated

ls/ls2 path [watch]
NodeChildrenChanged

类型
1. 父节点创建事件：NodeCreated
![QQ截图20180713084518.png](https://upload-images.jianshu.io/upload_images/7460499-a7b8e549c7e0f953.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

2. 修改父节点数据事件：NodeDataChanged
![QQ截图20180713084554.png](https://upload-images.jianshu.io/upload_images/7460499-9d0e2d75469e0032.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

3. 删除父节点事件：NodeDeleted
![QQ截图20180713084620.png](https://upload-images.jianshu.io/upload_images/7460499-2aab85ac5e00804a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

4. ls 为父节点设置 watcher，创建子节点触发：NodeChildrenChanged
![QQ截图20180713084643.png](https://upload-images.jianshu.io/upload_images/7460499-152725ec1d94b3e2.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

5. ls 为父节点设置 watcher，删除子节点触发：NodeChildrenChanged
![QQ截图20180713084709.png](https://upload-images.jianshu.io/upload_images/7460499-606ead3b977d0486.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

6. ls 为父节点设置 watcher，修改子节点不触发事件
![QQ截图20180713084735.png](https://upload-images.jianshu.io/upload_images/7460499-57e8cce60f065752.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 使用场景
1. 统一资源配置
![QQ截图20180713084808.png](https://upload-images.jianshu.io/upload_images/7460499-2abd5324dfa46035.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

