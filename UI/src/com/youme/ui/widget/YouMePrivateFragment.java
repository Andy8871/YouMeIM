package com.youme.ui.widget;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.youme.ui.R;
import com.youme.ui.adapter.YouMeChatRoomAdapter;
import com.youme.ui.adapter.YouMeContactAdapter;
import com.youme.ui.adapter.YouMeFaceImageAdapter;
import com.youme.ui.utils.YouMeButtonColorFilter;
import com.youme.ui.utils.YouMeChatMessage;
import com.youme.ui.utils.YouMeContact;

import android.R.integer;
import android.R.string;
import android.app.Activity;
import android.app.Fragment;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ImageSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.webkit.WebView.FindListener;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.ViewFlipper;

public class YouMePrivateFragment extends Fragment {
	public final static String TAG = YouMePrivateFragment.class.getSimpleName();
	public final static String DEFAULT_PATT = "\\[/..\\]|\\[/.\\]|\\[/...\\]";
	/**
	 * 表情选择面板每行容纳表情个数
	 */
	private final static int FACE_IMAGE_ONE_LINE = 10;	
	
	
	public final static int HANDLER_TAG_INPUT_IMAGE = 1;	
	
	private Activity mActivity;
	
	
	private ListView mContactsListView;
	private ArrayList<YouMeContact> mContacters = new ArrayList<YouMeContact>();
	private YouMeContactAdapter mContactsAdapter = null;

	
	private ListView mChatListView;
	private ArrayList<YouMeChatMessage> mMessages = new ArrayList<YouMeChatMessage>();
	private YouMeChatRoomAdapter mChatAdapter = null;

	
	private ArrayList<ArrayList<HashMap<String, Object>>> mListFacePapes = new ArrayList<ArrayList<HashMap<String,Object>>>();
	private ViewFlipper mFaceFilepper = null;
	
	private HashMap<String, Object> mFaceImages = new HashMap<String,Object>();
	
	private LinearLayout mFaceImagePanel;
	private ImageButton mSpeechButton;
	private ImageButton mFaceImageButton;
	private ImageButton mSenButton;
	private EditText mEditText;
	
	private Handler mImageInputHandler = new Handler(){

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case HANDLER_TAG_INPUT_IMAGE:{
				HashMap<String, Object> map = (HashMap<String, Object>)msg.obj;
				String text = (String)map.get(YouMeFaceImageAdapter.FACE_NAME);
				SpannableString string = changeToSpannableString(text, DEFAULT_PATT);
				//int imageId = (Integer)map.get(FaceImageAdapter.FACE_IMAGE);
				Log.i(TAG, "***********selet image:" + text);
				mEditText.append(string);
				
			}
				break;

			default:
				break;
			}
		}
		
	};
	
	
	private String mMsgContent;
	/*
	 * private
	 */
	@Override
	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.fragmen_private, container, false);
		view.setOnTouchListener(new TouchEventListener());
		mFaceImagePanel = (LinearLayout) view.findViewById(R.id.faceimagepanel);
		if (null != mFaceImagePanel) {
			mFaceImagePanel.setVisibility(View.GONE);
		}
		mSpeechButton = (ImageButton) view.findViewById(R.id.speechbtn);
		if (null != mSpeechButton) {
			YouMeButtonColorFilter.setButtonStateChangeListener(mSpeechButton);
			mSpeechButton.setOnClickListener(mBtnClickListener);
		}

		mFaceImageButton = (ImageButton) view.findViewById(R.id.imagebtn);
		if (null != mFaceImageButton) {
			YouMeButtonColorFilter.setButtonStateChangeListener(mFaceImageButton);
			mFaceImageButton.setOnClickListener(mBtnClickListener);
		}

		mSenButton = (ImageButton) view.findViewById(R.id.sendbtn);
		if (null != mSenButton) {
			YouMeButtonColorFilter.setButtonStateChangeListener(mSenButton);
			mSenButton.setOnClickListener(mBtnClickListener);
		}
		
		mFaceFilepper = (ViewFlipper)view.findViewById(R.id.faceFlipper);
		
		
		mEditText = (EditText)view.findViewById(R.id.inputedit);
		return view;
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		initContacter();

		mActivity = getActivity();
		mContactsListView = (ListView) mActivity
				.findViewById(R.id.list_contacts);
		mContactsAdapter = new YouMeContactAdapter(mActivity, R.layout.contacter_item,
				mContacters);
		mContactsListView.setAdapter(mContactsAdapter);
		mContactsListView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				Log.i(TAG, "onItemClick: " + position);
				mContactsAdapter.selectContact(position);
			}
		});
		initMessage();
		mContactsListView = (ListView) mActivity
				.findViewById(R.id.list_chatcontent);
		mChatAdapter = new YouMeChatRoomAdapter(mActivity,
				R.layout.chatmessage_item, mMessages);
		mContactsListView.setAdapter(mChatAdapter);
		mContactsListView.setOnItemClickListener(chatListOnItemClickListener);
		
	}

	private void initContacter() {
		
	}

	private void initMessage() {
		

	}

	private final OnClickListener mBtnClickListener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			int btnId = v.getId();
			switch (btnId) {
			case R.id.speechbtn:{
				YouMeChatMessage msg = new YouMeChatMessage(YouMeChatMessage.MSG_OWNER_RECV, YouMeChatMessage.MSG_TYPE_AUDIO, "����", "NULL");
				mMessages.add(msg);
				mChatAdapter.notifyDataSetChanged();
			}
				break;
			case R.id.imagebtn:
				if (!mFaceImagePanel.isShown()){
					addGridData();
					addGridView();
					mFaceImagePanel.setVisibility(View.VISIBLE);
				}
				else if (mFaceImagePanel.isShown())
					mFaceImagePanel.setVisibility(View.GONE);
				break;
			case R.id.sendbtn:{
				if (mFaceImagePanel.isShown())
					mFaceImagePanel.setVisibility(View.GONE);
				mMsgContent = mEditText.getText().toString();
				YouMeChatMessage msg = new YouMeChatMessage(YouMeChatMessage.MSG_OWNER_SEND, YouMeChatMessage.MSG_TYPE_TEXT, "����", mMsgContent);
				mMessages.add(msg);
				mChatAdapter.notifyDataSetChanged();
				//mChatAdapter.addMessage(msg);
				mEditText.setText("");
			}
				break;
			default:
				break;
			}
		}

	};
	
	private void addGridData()
	{
		if (mListFacePapes == null)
			return;
		
		TypedArray faceImages = getResources().obtainTypedArray(R.array.faceimage);
		String [] faceNames = getResources().getStringArray(R.array.facename);
		mListFacePapes.clear();
		//ArrayList<HashMap<String, Object>> listInPage = new ArrayList<HashMap<String,Object>>();
		for (int i = 0; i < faceNames.length; ++i){
			if ((i % 14) == 0){
				ArrayList<HashMap<String, Object>> list = new ArrayList<HashMap<String,Object>>();
				mListFacePapes.add(list);
			}
			HashMap<String, Object> map = new HashMap<String, Object>();
			map.put(YouMeFaceImageAdapter.FACE_NAME, faceNames[i]);
			map.put(YouMeFaceImageAdapter.FACE_IMAGE, faceImages.getResourceId(i, 0));
			Log.i(TAG, "put hashmap:" + faceNames[i] + ", " + faceImages.getResourceId(i, 0));
			mListFacePapes.get(i / 14).add(map);
			mFaceImages.put(faceNames[i], faceImages.getResourceId(i, 0));
		}
		Log.i(TAG, "list face papes:" + mListFacePapes.size());
	}
	
	private void addGridView()
	{
		if (null == mFaceFilepper)
			return;
		if (null == mActivity)
			return;
		
		mFaceFilepper.removeAllViews();
		for (int i = 0; i < mListFacePapes.size(); ++i)
		{
			View layoutView = LayoutInflater.from(mActivity).inflate(R.layout.faceimageview, null);
			GridView gridView = (GridView)layoutView.findViewById(R.id.GridView);
			gridView.setNumColumns(FACE_IMAGE_ONE_LINE);
			gridView.setSelector(new ColorDrawable(Color.TRANSPARENT));
			YouMeFaceImageAdapter adapter = new YouMeFaceImageAdapter(mActivity, mListFacePapes.get(i), R.layout.faceimage_item, new String[]{YouMeFaceImageAdapter.FACE_IMAGE}, new int[]{R.id.faceimage_item_view});
			adapter.setHandler(mImageInputHandler);
			gridView.setAdapter(adapter);
			mFaceFilepper.addView(layoutView);
		}
	}
	
	private class TouchEventListener implements OnTouchListener{

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			if (v.getId() != R.id.faceimagepanel){
				if (event.getAction() == MotionEvent.ACTION_DOWN)
				{
					if (mFaceImagePanel.isShown())
						mFaceImagePanel.setVisibility(View.GONE);
				}
			}
			
			return false;
		}
		
	}
	
	
	private SpannableString changeToSpannableString(String content, String patt)
	{
		if (null == patt)
			return new SpannableString(content);
		SpannableString result = new SpannableString(content);
		Pattern pattern = Pattern.compile(patt);
		Matcher matcher = pattern.matcher(content);
		while (matcher.find()){
			String imageName = matcher.group();
			int start =  matcher.start();
			int end = matcher.end();
			int imageId = (Integer)mFaceImages.get(imageName);
			Bitmap bitmap = BitmapFactory.decodeResource(getResources(), imageId);
			ImageSpan imageSpan = new ImageSpan(mActivity, bitmap);
			result.setSpan(imageSpan, start, end, Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
		}
		return result;
	}
	
	private OnItemClickListener chatListOnItemClickListener = new OnItemClickListener() {

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			Log.d(TAG, "Chat list select the item:" + position);
			
		}
	};
}
