package com.youme.ui.adapter;

import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.youme.ui.R;
import com.youme.ui.utils.YouMeChatMessage;
import com.youme.ui.utils.YouMeFaceManager;
import com.youme.ui.widget.YouMeGifDrawable;
import com.youme.ui.widget.YouMeAnimationImageSpan;
import com.youme.ui.widget.YouMePrivateFragment;
import com.youme.ui.widget.YouMeGifDrawable.UpdateListener;

import android.R.integer;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.Drawable;
import android.text.SpannableString;
import android.text.Spanned;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class YouMeChatRoomAdapter extends ArrayAdapter<YouMeChatMessage> {
	private Context mContext;
	private int mResourceId;
	private LayoutInflater mLayoutInflater;
	private List<YouMeChatMessage> mMessages;
	private final static String AUDIO_REPLACE_STRING = "Audio";

	public YouMeChatRoomAdapter(Context context, int resource,
			List<YouMeChatMessage> objects) {
		super(context, resource, objects);
		mContext = context;
		mResourceId = resource;
		mLayoutInflater = LayoutInflater.from(context);
		mMessages = objects;
	}

	/*
	 * public ChatMessageAdapter(Context context, int resource, ChatMessage[]
	 * objects) { super(context, resource, objects); mResourceId = resource;
	 * mLayoutInflater = LayoutInflater.from(context); }
	 */

	public void addMessage(YouMeChatMessage msg) {
		mMessages.add(msg);
		notifyDataSetChanged();
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View view = null;
		ViewHolder holder = null;
		if (null == convertView) {
			holder = new ViewHolder();
			view = mLayoutInflater.inflate(R.layout.chatmessage_item, null);
			holder.sendLayout = (RelativeLayout) view
					.findViewById(R.id.send_layout);
			holder.sendTextView = (TextView) view.findViewById(R.id.send_msg);
			holder.sendHead = (ImageView) view.findViewById(R.id.send_head);
			holder.sendOwnerName = (TextView) view.findViewById(R.id.send_name);
			holder.sendAudioBackground = (RelativeLayout) view
					.findViewById(R.id.send_AudioBackground);
			holder.sendAudio = (ImageView) view.findViewById(R.id.send_audio);

			holder.recvLayout = (RelativeLayout) view
					.findViewById(R.id.recv_layout);
			holder.recvTextView = (TextView) view.findViewById(R.id.recv_msg);
			holder.recvHead = (ImageView) view.findViewById(R.id.recv_head);
			holder.recvOwnerName = (TextView) view.findViewById(R.id.recv_name);
			holder.recvAudioBackground = (RelativeLayout) view
					.findViewById(R.id.recv_AudioBackground);
			holder.recvAudio = (ImageView) view.findViewById(R.id.recv_audio);
			view.setTag(holder);
		} else {
			view = convertView;
			holder = (ViewHolder) view.getTag();
		}
		YouMeChatMessage msg = getItem(position);
		switch (msg.getMsgType()) {
		case YouMeChatMessage.MSG_TYPE_AUDIO: {
			if (msg.getMsgOwner() == YouMeChatMessage.MSG_OWNER_RECV) {
				holder.sendLayout.setVisibility(View.GONE);
				holder.recvLayout.setVisibility(View.VISIBLE);
				holder.recvOwnerName.setText(msg.getOwnerName());
				holder.recvTextView.setVisibility(View.GONE);
				holder.recvAudioBackground.setVisibility(View.VISIBLE);
				holder.recvAudio
						.setBackgroundResource(R.drawable.recvaudioanimation);
				//holder.recvAudio.setOnClickListener(mImageOnclickListener);
			} else if (msg.getMsgOwner() == YouMeChatMessage.MSG_OWNER_SEND) {
				holder.recvLayout.setVisibility(View.GONE);
				holder.sendLayout.setVisibility(View.VISIBLE);
				holder.sendOwnerName.setText(msg.getOwnerName());
			}
		}
			break;
		case YouMeChatMessage.MSG_TYPE_TEXT: {
			if (msg.getMsgOwner() == YouMeChatMessage.MSG_OWNER_RECV) {
				holder.sendLayout.setVisibility(View.GONE);
				holder.recvLayout.setVisibility(View.VISIBLE);
				holder.recvTextView
						.setText(convertNormalStringToSpannableString(
								msg.getMsgText(), holder.recvTextView));
				holder.recvOwnerName.setText(msg.getOwnerName());
			} else if (msg.getMsgOwner() == YouMeChatMessage.MSG_OWNER_SEND) {
				holder.recvLayout.setVisibility(View.GONE);
				holder.sendLayout.setVisibility(View.VISIBLE);
				holder.sendTextView
						.setText(convertNormalStringToSpannableString(
								msg.getMsgText(), holder.sendTextView));
				holder.sendOwnerName.setText(msg.getOwnerName());
			}
		}
			break;
		default:
			break;
		}

		return view;
	}

	/**
	 * check the cacke first
	 * 
	 * @param message
	 * @param tv 
	 * @return
	 */
	private SpannableString convertNormalStringToSpannableString(
			String message, final TextView tv) {
		SpannableString value = SpannableString.valueOf(message);
		Pattern pattern = Pattern.compile(YouMePrivateFragment.DEFAULT_PATT);
		Matcher localMatcher = pattern.matcher(value);
		while (localMatcher.find()) {
			String imageName = localMatcher.group(0);
			int k = localMatcher.start();
			int m = localMatcher.end();
			if (m - k < 8) {
				AssetManager am = mContext.getAssets();
				String fileName = YouMeFaceManager.getInstance().getGifPath(
						imageName);
				InputStream inputStream = null;
				try {
					if (null != fileName)
						inputStream = am.open(fileName);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (null != inputStream) {// wrapping with weakReference
					WeakReference<YouMeAnimationImageSpan> localImageSpanRef = new WeakReference<YouMeAnimationImageSpan>(
							new YouMeAnimationImageSpan(new YouMeGifDrawable(
									inputStream,
									new YouMeGifDrawable.UpdateListener() {
										@Override
										public void update() {// update the
																// textview
											tv.postInvalidate();
										}
									})));
					value.setSpan(localImageSpanRef.get(), k, m,
							Spanned.SPAN_INCLUSIVE_INCLUSIVE);
				}
			}
		}
		return value;
	}


	/*private OnClickListener mImageOnclickListener = new OnClickListener() {

		@Override
		public void onClick(View v) {
			//v.setBackgroundColor(color);
			AnimationDrawable drawable = (AnimationDrawable) ((ImageView) v)
					.getBackground();
			if (null != drawable) {
				if (!drawable.isRunning())
					drawable.start();
				else {
					drawable.selectDrawable(0);
					drawable.stop();
				}
			}

		}

	};*/

	class ViewHolder {
		RelativeLayout sendLayout;
		TextView sendTextView;
		ImageView sendAudio;
		TextView sendOwnerName;
		RelativeLayout sendAudioBackground;
		ImageView sendHead;

		RelativeLayout recvLayout;
		TextView recvTextView;
		ImageView recvAudio;
		TextView recvOwnerName;
		RelativeLayout recvAudioBackground;
		ImageView recvHead;
	}

}
