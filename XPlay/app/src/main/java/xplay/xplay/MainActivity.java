package xplay.xplay;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;

public class MainActivity extends AppCompatActivity implements Runnable, SeekBar.OnSeekBarChangeListener {
    private static final int REQUEST_PERMISSIONS_CODE = 1;
    private static final String[] REQUIRED_PERMISSIONS = {Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE};

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static Button button;
    private static SeekBar seekBar;
    private static Thread thread;

    public static void ShowOrHideUI()
    {
        if (IsPause())
        {
            seekBar.setAlpha(1);
            button.setAlpha(1);
        }
        else
        {
            seekBar.setAlpha(0);
            button.setAlpha(0);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!isRequiredPermissionsGranted() && Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(REQUIRED_PERMISSIONS, REQUEST_PERMISSIONS_CODE);
        }

        // 去掉标题栏
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        // 全屏，隐藏状态
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        // 屏幕为横屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.activity_main);
        button = findViewById(R.id.open_button);
        seekBar = findViewById(R.id.aplayseek);
        seekBar.setMax(1000);
        seekBar.setOnSeekBarChangeListener(this);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("XPlay","open button click!");
                //打开选择路径窗口
                Intent intent = new Intent();
                intent.setClass(MainActivity.this, OpenUrl.class);
                startActivity(intent);
                ShowOrHideUI();
            }
        });

        // 启动播放进度线程
        thread = new Thread(this);
        thread.start();
    }

    @Override
    public void run() {
        for(;;) {
            seekBar.setProgress((int)(PlayPos() * 1000));
            try {
                Thread.sleep(40);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        ShowOrHideUI();
    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        Seek((double)seekBar.getProgress() / (double)seekBar.getMax());
        ShowOrHideUI();
    }

    private boolean isRequiredPermissionsGranted() {
        for (String permission : REQUIRED_PERMISSIONS) {
            if (ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_DENIED) {
                return false;
            }
        }
        return true;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native double PlayPos();
    public native void Seek(double pos);
    public native static boolean IsPause();
}

