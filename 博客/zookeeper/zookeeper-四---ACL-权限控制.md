## 权限控制

ACL：Access Control List，访问控制列表，是前几年盛行的一种权限设计，它的核心在于用户直接和权限挂钩。
RBAC：Role Based Access Control ，翻译过来基本上就是基于角色的访问控制系统。

RBAC的核心是用户只和角色关联，而角色代表对了权限，这样设计的优势在于使得对用户而言，只需角色即可以，而某角色可以拥有各种各样的权限并可继承。ACL和RBAC相比缺点在于由于用户和权限直接挂钩，导致在授予时的复杂性，虽然可以利用组来简化这个复杂性，但仍然会导致系统不好理解，而且在取出判断用户是否有该权限时比较的困难，一定程度上影响了效率。

除两上述两种主要的模型之外，还有包括：基于属性的访问控制ABAC和基于策略的访问控制PBAC等等，但是应用不是很广泛。

[参考](https://www.cnblogs.com/hellokitty1/p/5916070.html)

## 正文

1. 针对节点可以设置相关的读写权限，目的是为了保障数据安全性
2. 权限 premissions 可以指定不同的权限范围以及角色

getAcl：获取指定节点的 acl 信息
setAcl：设置节点的 acl 权限信息
addauth：输入认证授权信息，注册时输入明文密码，但在 zk 系统里，密码以加密的形式存在。

### ACL 构成
acl 通过 [scheme:id:permissions] 构成

#### scheme
代表采用的某种权限机制

- world: world下只有一个 id，即只有一个用户，也就是 anyone，组合写法为：world:anyone:[permissions]
- anth: 代表认证登录，需要注册用户有权限，形式为：auth:user:password:[permissions]
- digest: 需要对密码加密才能访问，digest:username:BASE64(SHA1(password)):[permissions]
![QQ截图20180713085219.png](https://upload-images.jianshu.io/upload_images/7460499-7c57d00e7194ffda.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

- ip: 设置为指定的 ip，此时对 ip 访问进行限制，ip:192.168.1.1:[permissions]
- super: 代表超级管理员，拥有所有的权限

#### id
代表允许访问的用户

#### permission
权限组合字符串

缩写 crdwa
CREATE：创建子节点
READ：获取节点、子节点
WRITE：设置节点数据
DELETE：删除子节点
ADMIN：设置权限

### 命令行设置 ACL
![QQ截图20180713085417.png](https://upload-images.jianshu.io/upload_images/7460499-9c206c1422dfef47.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
![QQ截图20180713085444.png](https://upload-images.jianshu.io/upload_images/7460499-09b0bf720eaf0131.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 用户登录
addauth digest imooc:imooc
![QQ截图20180713085530.png](https://upload-images.jianshu.io/upload_images/7460499-113a942716963cec.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 超级用户 super
1. 修改 zkServer.sh 增加 super 管理员
2. 重启 zk 使生效
![QQ截图20180713085618.png](https://upload-images.jianshu.io/upload_images/7460499-544438b8cdd78e84.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### ACL 使用场景
1. 开发、测试环境分离，开发者无权操作测试库节点
2. 生产环境控制指定 ip 的服务可以访问相关节点，防止混乱


