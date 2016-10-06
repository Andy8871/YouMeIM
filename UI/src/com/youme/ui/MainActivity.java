package com.youme.ui;

import java.util.HashMap;

import com.youme.imengine.IMEngine;
import com.youme.ui.entries.YouMeIMManager;
import com.youme.ui.listener.OnLoginListener;
import com.youme.ui.widget.YouMeAllianceFragment;
import com.youme.ui.widget.YouMePrivateFragment;
import com.youme.ui.widget.YouMeSystemFragment;
import com.youme.ui.widget.YouMeWorldFragment;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
public class MainActivity extends Activity {
	private final static String TAG = MainActivity.class.getSimpleName();
	private HashMap<Integer, Fragment> mFragments = null;
	private Fragment mCurrentFragment = null;
	private ImageButton mExitButton = null;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		IMEngine.Init(this);
		super.onCreate(savedInstanceState);
		// getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
		// WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);
		mExitButton = (ImageButton) findViewById(R.id.exit);
		mExitButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				moveTaskToBack(true);
			}
		});
		initView();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private void initView() {
		YouMeIMManager imManager = YouMeIMManager.getInstance();
		imManager.initIM();
		String json = "{\"userid\":\"sjy\",\"msgType\":0,\"gender\":1,\"nick\":\"我是大宝哥哥15\",\"avatar\":\"http://touxiang.vipyl.com/attached/image/20131025/20131025173450685068.jpg\",\"level\":15,\"msg\":\"sdfsf\",\"vip\":13}";
		imManager.login(json, "深圳", "丐帮", new OnLoginListener() {
			@Override
			public void onLogion(String id, int errorCode, int status) {
				
			}
		});
		
		
		adjustView();
		TabClickListen clickListen = new TabClickListen();

		findViewById(R.id.check_system).setOnClickListener(clickListen);
		findViewById(R.id.check_world).setOnClickListener(clickListen);
		findViewById(R.id.check_private).setOnClickListener(clickListen);
		findViewById(R.id.check_guild).setOnClickListener(clickListen);
		initFragmentList();
		onSwitchFragment(R.id.check_system);

	}

	private void adjustView() {
		Bitmap bmp = BitmapFactory.decodeResource(getResources(),
				R.drawable.youme_im_background);
		Log.i(TAG, "background widthd:" + bmp.getWidth() + ", height:"
						+ bmp.getHeight());
		FrameLayout frameLayout = (FrameLayout) findViewById(R.id.fragment_container);
		RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) frameLayout
				.getLayoutParams();
		params.width = bmp.getWidth();
		params.height = bmp.getHeight();
		frameLayout.setLayoutParams(params);
		frameLayout.requestLayout();
	}

	@SuppressLint("UseSparseArrays")
	private void initFragmentList() {
		
		mFragments = new HashMap<Integer, Fragment>(4);
		mFragments.put(R.id.check_system, new YouMeSystemFragment());
		mFragments.put(R.id.check_world, new YouMeWorldFragment());
		mFragments.put(R.id.check_private, new YouMePrivateFragment());
		mFragments.put(R.id.check_guild, new YouMeAllianceFragment());
	}

	private void onSwitchFragment(int id) {
		FragmentManager fm = getFragmentManager();
		if (null == fm) {
			Log.e(TAG, "getFragmentManager is null");
			return;
		}
		FragmentTransaction ft = getFragmentManager().beginTransaction();
		if (null != mCurrentFragment) {
			ft.hide(mCurrentFragment);
		}
		Fragment fragment = fm.findFragmentByTag(mFragments.get(id).getClass()
				.getSimpleName());

		if (null == fragment)
			fragment = mFragments.get(id);


		mCurrentFragment = fragment;

		if (!fragment.isAdded()) {
			ft.add(R.id.fragment_container, fragment, fragment.getClass()
					.getSimpleName());
		} else {
			ft.show(fragment);
		}
		ft.commit();
	}

	private class TabClickListen implements OnClickListener {

		@Override
		public void onClick(View v) {
			Log.d(TAG, "tab check :" + v.getTag());
			onSwitchFragment(v.getId());

		}

	}

}
