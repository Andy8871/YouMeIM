package com.youme.ui.utils;

import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.drawable.Drawable;
import android.support.annotation.NonNull;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class YouMeButtonColorFilter {
	private final static float[] BUTTON_PRESSED = {
		1.5f, 0, 0, 0, -20.0f,
		0, 1.5f, 0, 0, -20.0f,
		0, 0, 2.0f, 0, -20.0f,
		0, 0, 0, 2.0f, 0
		
	};
	
	private final static float[] BUTTON_RELEASE = {
		1.0f, 0, 0, 0, 0,
		0, 1.0f, 0, 0, 0,
		0, 0, 1.0f, 0, 0,
		0, 0, 0, 1.0f, 0
		
	};
	
	private static final OnTouchListener mTouchListener = new OnTouchListener() {
		
		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (event.getAction() == MotionEvent.ACTION_DOWN)
			{
				//v.getBackground().setColorFilter(new ColorMatrixColorFilter(BUTTON_PRESSED));
				//v.getBackground().setColorFilter(Color.GRAY,PorterDuff.Mode.MULTIPLY);
				//v.setBackground(v.getBackground());
				Drawable drawable = v.getBackground();
				if (drawable != null)
					drawable.setColorFilter(Color.GRAY,PorterDuff.Mode.MULTIPLY);
			}
			else if (event.getAction() == MotionEvent.ACTION_UP)
			{
				//v.getBackground().setColorFilter(new ColorMatrixColorFilter(BUTTON_RELEASE));
				//v.setBackground(v.getBackground());
				Drawable drawable = v.getBackground();
				if (drawable != null)
					drawable.clearColorFilter(); 
			}
			return false;
		}
	};
	
	public static void setButtonStateChangeListener(@NonNull View v){
		v.setOnTouchListener(mTouchListener);
	}
}
