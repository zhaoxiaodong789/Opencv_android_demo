package com.example.opencv;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity  extends AppCompatActivity implements View.OnClickListener {
    private ImageView imageView;
    private TextView write;
    private TextView show;

    static {//加载so库
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView = findViewById(R.id.imageView);
        findViewById(R.id.show).setOnClickListener(this);
        findViewById(R.id.process).setOnClickListener(this);
        findViewById(R.id.button).setOnClickListener(this);
        write = findViewById(R.id.editText);
        show = findViewById(R.id.textView);

    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.show) {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test);
            imageView.setImageBitmap(bitmap);

        }else if(v.getId() == R.id.button){
            //选择图片
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.barcode);

            //条形码译码
            String s = getBarcode(bitmap);
            imageView.setImageBitmap(bitmap);
            String b = write.getText().toString();


            show.setText(s);
        }

        else {
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.test);
            getEdge(bitmap);
            imageView.setImageBitmap(bitmap);
        }
    }

    //获得Canny边缘
    native void getEdge(Object bitmap);
    native String getBarcode(Object bitmap);
}
