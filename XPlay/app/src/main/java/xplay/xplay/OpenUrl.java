package xplay.xplay;

import android.Manifest;
import android.content.pm.ActivityInfo;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class OpenUrl extends AppCompatActivity {
    private static final int GET_RECODE_AUDIO = 1;
    private static String[] PERMISSION_ALL = {
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE,
    };

    //音频输入-麦克风
    private final static int AUDIO_INPUT = MediaRecorder.AudioSource.MIC;
    //采用频率
    //44100是目前的标准，但是某些设备仍然支持22050，16000，11025
    //采样频率一般共分为22.05KHz、44.1KHz、48KHz三个等级
    private final static int AUDIO_SAMPLE_RATE = 44100;
    //声道 单声道
    private final static int AUDIO_CHANNEL = AudioFormat.CHANNEL_IN_MONO;
    //编码
    private final static int AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    // 缓冲区字节大小
    private int bufferSizeInBytes = 0;

    private AudioRecord audioRecord = null;

    private Button buttonFile;
    private Button buttonRtmp;
    Thread audioThread = null;
    private boolean isAudioRecording = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // 屏幕为横屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.openurl);
        buttonFile = findViewById(R.id.playvideo);
        buttonFile.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    EditText t = findViewById( R.id.fileurl );
                    //用户输入的URL，打开视频
                    Open(t.getText().toString());

                    //关闭当前窗口
                    finish();
                }
            }
        );

        buttonRtmp = findViewById(R.id.playrtmp);
        buttonRtmp.setOnClickListener(
            new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    EditText t = findViewById(R.id.rtmpurl);
                    //用户输入的URL，打开视频
//                    Open(t.getText().toString());
                    startAudioRecord();
                    //关闭当前窗口
                    finish();
                }
            }
        );
    }

    public void startAudioRecord() {
        isAudioRecording = true;
        bufferSizeInBytes = AudioRecord.getMinBufferSize(AUDIO_SAMPLE_RATE, AUDIO_CHANNEL, AUDIO_ENCODING);
        Log.i("bufferSizeInBytes", String.valueOf(bufferSizeInBytes));
        audioRecord = new AudioRecord(AUDIO_INPUT, AUDIO_SAMPLE_RATE, AUDIO_CHANNEL, AUDIO_ENCODING, bufferSizeInBytes);
        audioRecord.startRecording();

        audioThread = new Thread(new Runnable() {
            @Override
            public void run() {
                // 监听音频数据
                byte[] audioData = new byte[bufferSizeInBytes];
                int readSize = 0;

                while (isAudioRecording) {
                    readSize = audioRecord.read(audioData, 0, bufferSizeInBytes);
                    if (AudioRecord.ERROR_INVALID_OPERATION != readSize) {
                        Log.i("readSize", String.valueOf(readSize));
                        Log.i("audioData", String.valueOf(audioData));
                        SetAudioData(audioData, readSize, System.currentTimeMillis());
                    } else {
                        Log.w("readSize", String.valueOf(readSize));
                    }
                }
            }
        });

        audioThread.start();
    }

    public void stopAudioRecord() throws InterruptedException {
        isAudioRecording = false;

        while (audioThread.isAlive())
        {
            Thread.sleep(10);
            Log.i("stopRecord", "1");
        }
        audioRecord.stop();
        Log.i("stopRecord", "2");
    }

    public native void Open(String url);
    public native void SetAudioData(byte[] bytes, int size, long pts);
}
