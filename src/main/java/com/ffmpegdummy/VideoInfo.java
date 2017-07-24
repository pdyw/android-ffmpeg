package com.ffmpegdummy;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by yw on 2017/7/21.
 */

public class VideoInfo implements Parcelable{

    public int width;
    public int height;
    public int fps;
    public int totalMs;

    public VideoInfo(){

    }

    public VideoInfo(Parcel in){
        this.width=in.readInt();
        this.height=in.readInt();
        this.fps=in.readInt();
        this.totalMs=in.readInt();

    }

    private static final Creator<VideoInfo> CREATOR=new Creator<VideoInfo>() {
        @Override
        public VideoInfo createFromParcel(Parcel source) {
            return new VideoInfo(source);
        }

        @Override
        public VideoInfo[] newArray(int size) {
            return new VideoInfo[size];
        }
    };

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(width);
        dest.writeInt(height);
        dest.writeInt(fps);
        dest.writeInt(totalMs);
    }

    @Override
    public int describeContents() {
        return 0;
    }
}
