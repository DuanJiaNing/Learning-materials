![](http://upload-images.jianshu.io/upload_images/7460499-bd0bee8dcf4ce718.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
Android-用WebView开发简单的浏览器

Android 提供了 WebView 组件，WebView 本身就是一个浏览器实现。

### 例子中主要用到了 WebView 的以下方法：
- void loadUrl(String url) 加载指定 url 对应的网页
- void goBack() 后退
- void goForward() 前进
- boolean canGoBack() 根据`历史记录`判断是否可以回退
- boolean canGoForward() 根据`历史记录`判断是否可以前进
- WebSettings getSettings() 获得对 WebView 进行控制的 WebSettings 对象
（调用其`setJavaScriptEnabled`方法使`WebView`支持`JavaScript`）
- void setWebViewClient(WebViewClient client) WebViewClient 负责处理 
WebView 各种通知，请求，比如页面开始加载及加载完成、资源加载中、url已打开等
- void setWebChromeClient(WebChromeClient client) WebChromeClient主要辅助WebView处理Javascript的对话框、网站图标、网站title、**加载进度**等

### 浏览器功能概述：
1. 第一次打开应用时默认会把`百度`设置为首页，可通过底部`主页`按钮跳转到主页，或通过长按弹出对话框修改主页
2. `主页`的 URL 地址使用`SharedPreference`保存
3. 底部四个`ImageButton`从做往右依次为`主页` `刷新` `回退` `前进`
4. 顶部的`ActionBar`用`void setCustomView(View)`添加了一个自定义的用于`输入网址`的输入框和一个`转到`按钮，`转到`按钮默认是不可见且不可用的，当点击了用于`输入网址`的`EditText`就会显示并可用。
5. 用于显示网页加载进度的`ProgressBar`

### 截图
![这里写图片描述](http://upload-images.jianshu.io/upload_images/7460499-df28e493f6466990?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

### 代码部分
- 每次打开一个网页时会调用该方法，在这里控制`ActionBar`内的`Edittext`显示`加载中...`
``` java
 mWebView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                super.onPageStarted(view, url, favicon);
                mShowTitle.setText("加载中...");
            }
            ...
}
```
- 覆写`shouldOverrideUrlLoading`并返回`true`，否则 WebView 默认会调用系统的浏览器或第三方浏览器，而不使用当前activity中的`WebView`

``` java
mWebView.setWebViewClient(new WebViewClient() {
			...
			@Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);
                return true;
            }
		    ...
}
```
- 当网页加载进度改变时会回调`onProgressChanged`方法，在这里更新用于显示进度的`ProgressBar`。注意`newProgress`值为 0 到 100 ，因而将`ProgressBar`最大值设为 100 。
- `onReceivedTitle`返回当前加载网页的标题将标题显示在 actionBar 内的 EditText 上。
``` java
mWebView.setWebChromeClient(new WebChromeClient() {
            @Override
            public void onProgressChanged(WebView view, int newProgress) {
                mProgressBar.setProgress(newProgress);
                if (newProgress == 100) {
                    mProgressBar.postDelayed(() -> mProgressBar.setProgress(0), 2000);
                }
        }

            @Override
            public void onReceivedTitle(WebView view, String title) {
                super.onReceivedTitle(view, title);
                currentUrlTitle = title;
                mShowTitle.setText(currentUrlTitle);
            }
        });
```
- 监听物理按键`返回`的点击事件，如果`WebView`的`历史记录`里有上一页相关记录就调用`void goBack()`方法
``` java
@Override
    public void onBackPressed() {
        if (mWebView.canGoBack())
            mWebView.goBack();
        else finish();
    }
```
- 判断当前是否处于正在输入网址（actionBar上的 EditText）的状态
1 是的话调用此方法表示输入结束从使`转到`按钮消失并不可用
2 使`EditText`控件失去焦点，不然会一直显示光标，效果不好

``` java
private void finishEdit() {
        if (isEditFocus) {
            isEditFocus = false;
            mShowTitle.clearFocus();

            Animation animation = AnimationUtils.loadAnimation(MainActivity.this, android.R.anim.fade_out);
            animation.setFillAfter(true);
            mLoadUrl.startAnimation(animation);
            mLoadUrl.setEnabled(false);

            mShowTitle.setText(currentUrlTitle);
        }
    }
```
这个方法有三个地方调用到：
1 当正在输入网址时用户转移注意力到`WebView`上，并触摸到`WebView`，此时会先判断软键盘是否正在显示，正在显示就关闭软键盘然后使输入框失去焦点并隐藏`转到`按钮
``` java
 mWebView.setOnTouchListener((View view, MotionEvent event) -> {
       if (event.getAction() == MotionEvent.ACTION_DOWN) {
           finishEdit();
           InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
           if (imm.isActive())
               imm.hideSoftInputFromWindow(mShowTitle.getWindowToken(),0);

       }
       return false;
   });

```
2 当**正在输入**网址时点击了物理按键`返回`,此时`void onBackPressed()`方法是不会被调用的，因为此时软键盘正在显示，调用`boolean dispatchKeyEvent(KeyEvent event)`方法拦截`返回`按键的点击事件，关闭软键盘的同时使输入框失去焦点并隐藏`转到`按钮。

``` java
@Override
public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK)
            finishEdit();
        return super.dispatchKeyEvent(event);
    }
```
3  点击了 actionBar 处的`转到`按钮
3.1 此时要先判断是否正确输入，然后判断输入网址是否是新的（和当前正在显示的网页的网址不同），满足就让`WebView`加载该网页
3.2 判断软键盘是否正在显示，正在显示就关闭软键盘然后使输入框失去焦点并隐藏`转到`按钮
>以下代码包含所有按钮的点击事件，每个按钮都在xml中添加了
>`android:onClick="onClick"`属性
``` java
 public void onClick(View view) {
        switch (view.getId()) {
            case R.id.main_goBack:
                if (mWebView.canGoBack())
                    mWebView.goBack();
                break;
            case R.id.main_goForward:
                if (mWebView.canGoForward())
                    mWebView.goForward();
                break;
            case R.id.main_refresh:
                mWebView.loadUrl(currentUrl);
                break;
            case R.id.actionBar_goto:
                String url = mShowTitle.getText().toString() == "" ? null : mShowTitle.getText().toString().equals(currentUrl) ? null : mShowTitle.getText().toString();
                if (url != null)
                    mWebView.loadUrl(url);

                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm.isActive())
                    imm.hideSoftInputFromWindow(mShowTitle.getWindowToken(),0);

                finishEdit();
                break;
            case R.id.main_home:
                if (!currentUrl.equals(homeUrl))
                    mWebView.loadUrl(homeUrl);
                break;
        }
    }
```

### 值得注意的地方：
- 在style中使用`colorControlHighlight`属性改变默认的按钮点击波纹效果的颜色
比如这样：
``` xml
<item name="colorControlHighlight">@color/colorPrimary</item>
```

- 在按钮的xml属性中通过设置
``` xml 
style=`"@style/Base.Widget.AppCompat.Button.Borderless.Colored"
```
使按钮的点击效果不受布局边界限制。其他默认不可点击的view（如EditText，TextView等）还要在添加一个：

``` xml 
android:clickable="true"
```
- 使用到的图标在这里下载的：[Material icons - Material Design](https://material.io/icons/)
- 调用`ActionBar`的`setCustomView`方法前先要调用`setDisplayShowCustomEnabled`方法并置为true。
- 修改主页用到了对话框：
``` java
 AlertDialog.Builder builder
	 .setTitle("修改主页")
     .setNegativeButton("取消", (DialogInterface dialog, int which) -> dialog.dismiss())
```
这里本来是用`new DialogInterface.OnClickListener() {...}`的形式写的，后来改成使用`Lambda`的方式，这时要在app的`build.gradle`里加上：
``` 
android {
    ...
        jackOptions {
            enabled true
        }
        ...
    }
    ...
    compileOptions {
        targetCompatibility 1.8
        sourceCompatibility 1.8
    }
    ...
}
```
------
