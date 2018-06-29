![](http://upload-images.jianshu.io/upload_images/7460499-29ef1ba56d667935.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
android中AsyncTask异步任务的使用

### 1.关于AsyncTask
在android中实现异步任务有两种方法：一种为通过多线程Thread配合Handler实现，另一种就是通过android为我们提供的AsyncTask来实现。AsyncTask使得编写异步任务更加简单。
public abstract class AsyncTaskextends Object java.lang.Object ↳ android.os.AsyncTask<Params, Progress, Result>

Params代表：启动任务执行的输入参数
Progress代表：后台任务执行的进度
Result代表：后台计算结果的类型
泛型指定不是必须的，在特定情况下不需要用到的可以传Void类型代替。
###  2.相关的重要方法
通过自定义的类继承AsyncTask类并指定泛型对应类型创建自己的异步任务类 privateclassMyTaskextendsAsyncTask<Void,Void,Void>{...}

一个异步任务的执行一般包括以下几步：
#### 1.final[AsyncTask](http://www.android-doc.com/reference/android/os/AsyncTask.html)<Params, Progress, Result> 
execute(Params...);
此方法是异步任务的启动入口，该方法应该在UI线程中调用，调用此方法时会让任务以单线程队列方式或线程池队列的方式运行，具体取决于版本不同而不同，Parames对应定义子类时指定的Params，比如若指定为String类型，则MyTask.execute("afds","ffdf","www","ff");。
#### 2.void onPreExecute();
该方法在execute方法调用后立即执行，可以用来完成在开启线程执行异步任务之前的操作，如标记一个异步任务即将开始，如让一个ProgressBar控件可见。
#### 3.Result 
doInBackground(Params...);
AsyncTask子类必须覆写方法，该方法在AsyncTask创建的线程中执行，该方法在onPreExecute方法后立即执行，用于执行费事操作，类似Thread中的run方法。此方法接收输入参数（即execute中传入的参数），并在异步任务完成后返回结果，执行期间可以调用void publishProgress(Progress... values)方法实时将进度信息传递给void onProgressUpdate(Progress...values)方法（若调用了publishProgress方法则MyTask方法中也应覆写onProgressUpdate方法）；
#### 4.void onProgressUpdate(Progress...);
该方法在UI线程中执行，接收publishProgress方法传来的参数，更新UI。
#### 5.void onPostExecute(Result);
该方法在UI线程中执行，接收doInBackground方法返回的参数，处理异步所得结果。

注意事项：
1.异步任务的子类的实例应在UI线程中创建
2.除execute方法外，doInBackground、onPreExecute、onProgressUpdate。onPostExecute方法不能手动调用，而publishProgress方法只能在doInBackground方法中调用
3.doInBackground方法是在与UI线程不同的线程中执行的，因而不能再该方法中操作UI
4.一个任务实例只能执行一次
（以上内容转自：http://blog.csdn.net/liuhe688/article/details/6532519）

### 3.Demo
demo实现功能为从网络异步下载一张图片
```java
public class DemoActivity extends Activity {  
  
    private ProgressBar progressBar;  
  
    private ImageView imageView;  
  
    private TextView textView;  
  
    private Button button;  
  
    private final String url = "http://www.bz55.com/uploads/allimg/150720/139-150H0110922.jpg";  
  
    @Override  
    protected void onCreate(Bundle savedInstanceState) {  
        super.onCreate(savedInstanceState);  
        setContentView(R.layout.demo_layout);  
  
        progressBar = (ProgressBar) findViewById(R.id.demo_progressBar);  
        progressBar.setVisibility(View.INVISIBLE);  
        imageView = (ImageView) findViewById(R.id.demo_image);  
        textView = (TextView) findViewById(R.id.demo_showProgress);  
        button = (Button) findViewById(R.id.demo_button);  
        button.setOnClickListener(new View.OnClickListener() {  
            @Override  
            public void onClick(View view) {  
                imageView.setImageBitmap(null);  
                new MyTask().execute(url);  
            }  
        });  
    }  
  
    private class MyTask extends AsyncTask<String,String,Bitmap>  
    {  
  
        @Override  
        protected void onPreExecute() {  
            super.onPreExecute();  
            textView.setText("异步开始");  
            progressBar.setVisibility(View.VISIBLE);  
        }  
  
        @Override  
        protected Bitmap doInBackground(String... strings) {  
            publishProgress("正在下载图片");  
            return getBitmapFromUrl(strings[0]);  
        }  
  
        @Override  
        protected void onProgressUpdate(String... values) {  
            super.onProgressUpdate(values);  
            textView.setText(values[0]);  
        }  
  
        @Override  
        protected void onPostExecute(Bitmap bitmap) {  
            super.onPostExecute(bitmap);  
            imageView.setImageBitmap(bitmap);  
            textView.setText("下载结束");  
            progressBar.setVisibility(View.INVISIBLE);  
        }  
  
    }  
  
  
    public Bitmap getBitmapFromUrl(String urlString) {  
        Bitmap bitmap;  
        InputStream is = null;  
  
        try {  
            URL url = new URL(urlString);  
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();  
            is = new BufferedInputStream(connection.getInputStream());  
            bitmap = BitmapFactory.decodeStream(is);  
            connection.disconnect();  
            //Log.i("image", "getBitmapFromUrl: true");  
            return bitmap;  
  
        } catch (MalformedURLException e) {  
            e.printStackTrace();  
        } catch (IOException e) {  
            e.printStackTrace();  
        } finally {  
            try {  
                is.close();  
            } catch (IOException e) {  
                e.printStackTrace();  
            }  
        }  
        return null;  
    }  
  
}  
```
布局
```xml
<?xml version="1.0" encoding="utf-8"?>  
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"  
    android:orientation="vertical"  
    android:layout_width="match_parent"  
    android:layout_height="match_parent"  
    >  
  
    <TextView  
        android:layout_marginTop="5dp"  
        android:textColor="@color/colorAccent"  
        android:textSize="20sp"  
        android:id="@+id/demo_showProgress"  
        android:gravity="center"  
        android:paddingBottom="10dp"  
        android:paddingTop="10dp"  
        android:text=".............................."  
        android:layout_width="match_parent"  
        android:layout_height="wrap_content"  
        />  
    <Button  
        android:layout_marginTop="5dp"  
        android:layout_marginRight="16dp"  
        android:layout_marginLeft="16dp"  
        android:id="@+id/demo_button"  
        android:text="获取图片"  
        android:layout_width="match_parent"  
        android:layout_height="wrap_content"  
        />  
    <FrameLayout  
        android:gravity="center"  
        android:layout_width="match_parent"  
        android:layout_height="match_parent">  
  
        <ImageView  
            android:id="@+id/demo_image"  
            android:layout_width="match_parent"  
            android:layout_height="match_parent"  
            />  
  
        <ProgressBar  
            android:id="@+id/demo_progressBar"  
            android:layout_width="wrap_content"  
            android:layout_height="wrap_content"  
            android:layout_gravity="center" />  
    </FrameLayout>  
</LinearLayout>  
```

别忘了添加网络访问权限
`<uses-permission android:name="android.permission.INTERNET"/>`
最终效果
![](http://upload-images.jianshu.io/upload_images/7460499-05a39164f91eb031?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

----
