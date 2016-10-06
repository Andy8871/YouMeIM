package com.youme.ui.widget;

import java.util.ArrayList;

import com.youme.ui.R;

import android.app.Activity;
import android.app.Fragment;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.Spanned;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class YouMeSystemFragment extends Fragment {
	private final static String TAG = YouMeSystemFragment.class.getSimpleName();
	private TextView imageView = null;
	private YouMeGifDrawable drawable = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.fragmen_system, container, false);
		return view;
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);
		Activity activity = getActivity();
	}

	
	private final class ClickListener implements OnClickListener{

		@Override
		public void onClick(View v) {
			Log.i(TAG, "+++++++++onClick++++++++++++++++");
			
			if (!drawable.isRunning())
				drawable.start();
			else {
				drawable.stop();
			}
		}
		
	}
	
	
}
