博文内容总结自文章：[浏览器家族的安全反击战](http://mp.weixin.qq.com/s?__biz=MzAxOTc0NzExNg==&mid=2665514143&idx=1&sn=28ea209c00309e6b93d8d1f76032d7a4&chksm=80d67cdcb7a1f5ca81d8d454a98af56d58b22f6058f100e21ff30e70867ea6e3e922a4f000bf&mpshare=1&scene=23&srcid=1219hjtmL7SXftuOoe4cEI3A#rd )

## 同源策略
同源是指{protocol,host,port} 这三个东西必须得一样！ 例如有这么一个网页： http://www.store.com/product/page.html， 下面的表格列出了各种不同情况：
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-e96e4e0c503569ad?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 使用同源策略的原因
1. Cookie失窃，非法网页通过javascript访问其他应用的服务器，获取到Cookie后伪装成用户进行非法操作。
2. 通过iframe伪装form，将信息提交到恶意action，隐私数据提交到了不法网站，信息被盗取。
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-e0b153323946f8eb?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 同源策略的限制
**有src属性的标签不受同源策略的影响**。比如script、img、iframe、link。相当于浏览器发送get请求。原页面的js不能访问通过src加载的资源。

如果两个网页的**一级域名是相同的，他们可以共享cookie, 不过cookie的domain一定要设置为那个一级域名才可以**，例如：
”document.cookie = 'test=true;path=/;domain=store.com'”

对于浏览器来说：除了DOM、Cookie、LocalStorage、XMLHttprequest（Ajax）会受到同源策略的限制外，浏览器加载的第三方插件也有各自的同源策略。例如：flash,java applet,silverlight等。

既然服务器（domain）之间是互信的，那一个服务器(domain)可以**设置一个白名单，里边列出它允许哪些服务器(domain)的AJAX请求**。假设movie.com的白名单中有book.com， 那当属于book.com的JavaScript试图访问movie.com的时候，book.com先询问movie的白名单中是否有自己（是否允许自己访问），有就继续访问。
*这个方法叫做Cross Origin Resource Sharing，简称CORS，只不过这个方法需要服务器的配合。*

## jsonp 
jsonp 的原理很简单，利用了【前端请求静态资源的时候不存在跨域问题】这个思路，但是只支持 get。既然这个方法叫 jsonp，后端数据一定要使用 json 数据。
### 前端 jQuery 写法
```javascript
   $.ajax({  
	        url:"http://crossdomain.com/services.php",  
	        dataType:'jsonp',  
	        data:'',  
	        jsonp:'callback',  
	        success:function(result) {  
	            for(var i in result) {  
	                alert(i+":"+result[i]);//循环输出a:1,b:2,etc.  
	            }  
        },  
	        timeout:3000  
    });  

```
### 后端 SpringMVC 配置
```java
@ControllerAdvice
public class JsonpAdvice extends AbstractJsonpResponseBodyAdvice {
    public JsonpAdvice(){
        // 这样如果请求中带 callback 参数，Spring 就知道这个是 jsonp 的请求了
        super("callback");
    }
}
```
以上写法要求 SpringMVC 版本不低于 3.2 。
### 原理
首先在客户端注册一个callback方法（javaScript定义一个指定名字的回调方法）, 然后把callback的名字传给服务器。服务器先生成 json 数据。然后以 javascript 语法的方式，生成一个function , function 名字就是传递上来的回调函数的名字。

最后将 json 数据直接以入参的方式，放置到 function 中，这样就生成了一段 js 语法的文档，返回给客户端。

客户端浏览器，解析script标签，并执行返回的 javascript 文档，此时数据作为参数，传入到了客户端预先定义好的 callback 函数里.（动态执行回调函数）

## CORS
Cross-Origin Resource Sharing

通过服务器端中转，例如你是来自book.com的， 现在想访问movie.com，那可以让那个book.com把请求转发给movie.com！利用了代理模式，book.com就是代理人。但在正式访问之前需要先发送预检请求（preflight request）确认自己能访问。

跨域资源共享标准新增了一组 HTTP 首部字段，允许服务器声明哪些源站有权限访问哪些资源。另外，规范要求，对那些可能对服务器数据产生副作用的 HTTP 请求方法（特别是 GET 以外的 HTTP 请求，或者搭配某些 MIME 类型的 POST 请求），浏览器必须首先使用 OPTIONS 方法发起一个**预检请求（preflight request），从而获知服务端是否允许该跨域请求**。服务器确认允许之后，才发起实际的 HTTP 请求。在预检请求的返回中，服务器端也可以通知客户端，是否需要携带身份凭证（包括 Cookies 和 HTTP 认证相关数据）。参考文章：[HTTP访问控制（CORS）](https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Access_control_CORS)

![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-aa9cab4a01da2627?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

CORS需要服务器的配合，CORS 支持以下浏览器，不过目前来看，浏览器的问题已经越来越不重要了。
Chrome 3+
Firefox 3.5+
Opera 12+
Safari 4+
Internet Explorer 8+
### 前端 jQuery 写法
```jQuery
$.ajax({
    type: "POST",
    url: "...url...",
    dataType: 'json',
    crossDomain: true,
    xhrFields: {
        withCredentials: true
    },
    data: {
        name: "name_from_frontend"
    },
    success: function (response) {
        console.log(response)// 返回的 json 数据
        $("#response").val(JSON.stringify(response));
    }
});

```
crossDomain: true，这里代表使用跨域请求
xhrFields: {withCredentials: true}，这样配置就可以把 cookie 带过去了，不然 session 无法维护（cookie中保存了JSESSIONID），如果没有这个需求，也就不需要配置。

