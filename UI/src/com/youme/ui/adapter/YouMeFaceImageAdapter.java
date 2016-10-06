package com.youme.ui.adapter;

import java.util.ArrayList;
import java.util.HashMap;

import com.youme.ui.R;
import com.youme.ui.widget.YouMePrivateFragment;

import android.R.integer;
import android.R.string;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.ImageButton;;

public class YouMeFaceImageAdapter extends BaseAdapter {
	public final static String FACE_NAME = "faceName";
	public final static String FACE_IMAGE = "faceImage";
	private Context mContext;
	private ArrayList<HashMap<String, Object>> mList;
	private int mResouceId;
	private String[] mFrom;
	private int[] mTo;
	private Handler mHandler;
	public YouMeFaceImageAdapter(){
		
	}
	
	public YouMeFaceImageAdapter(Context context, ArrayList<HashMap<String, Object>> list, int resource, String[] from, int [] to){
		mContext = context;
		mList = list;
		mResouceId = resource;
		mFrom = from;
		mTo = to;
	}
	@Override
	public int getCount() {
		return mList.size();
	}

	@Override
	public Object getItem(int position) {
		return mList.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder = null;
		View view = null;
		if (null == convertView){
			holder = new ViewHolder();
			view = LayoutInflater.from(mContext).inflate(mResouceId, null);
			holder.imageView = (ImageButton)view.findViewById(mTo[0]);
			view.setTag(holder);
		}else {
			view = convertView;
			holder = (ViewHolder)view.getTag();
		}
		
		holder.imageView.setImageResource((Integer)mList.get(position).get(mFrom[0]));
		holder.imageView.setOnClickListener(new ImageItemOnCickListenr(position));
		Log.i("FregmentPrivate", "");
		return view;
	}
	
	public void setHandler(Handler handler){
		mHandler = handler;
	}
	
	class ViewHolder{
		ImageButton imageView;
	}
	
	private class ImageItemOnCickListenr implements OnClickListener{
		/**
		 * ���ڻ�ȡѡ�еı�����Ϣ
		 */
		private int mPosition;
		public ImageItemOnCickListenr(int position)
		{
			mPosition = position;
		}
		@Override
		public void onClick(View v) {
			//Log.i(FragmentPrivate.TAG, "**************************************************");
			HashMap<String, Object> map = mList.get(mPosition);
			Message msg = mHandler.obtainMessage();
			msg.obj = map;
			msg.what = YouMePrivateFragment.HANDLER_TAG_INPUT_IMAGE;
			msg.sendToTarget();
			//Log.i(FragmentPrivate.TAG, "imageName:" + map.get(FACE_NAME) + ", imageId:" + map.get(FACE_IMAGE));
		}
		
	}
	
}

