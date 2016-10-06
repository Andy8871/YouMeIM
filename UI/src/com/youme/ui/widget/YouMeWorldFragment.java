package com.youme.ui.widget;

import java.util.ArrayList;
import com.youme.ui.R;
import com.youme.ui.adapter.YouMeChatRoomAdapter;
import com.youme.ui.entries.YouMeIMManager;
import com.youme.ui.utils.YouMeChatMessage;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;

public class YouMeWorldFragment extends Fragment {
	private final static String TAG = YouMeWorldFragment.class.getSimpleName();
	private ListView chatRoomView = null;
	private LinearLayout inputRegion = null;
	private ArrayList<YouMeChatMessage> chatMessages = new ArrayList<YouMeChatMessage>();
	private YouMeChatRoomAdapter adapter = null;
	private EditText inputEdit = null;
	private final static int SPEECH_BUTTON = R.id.speechbtn;
	private final static int EMOTION_BUTTON = R.id.imagebtn;
	private final static int SEND_BUTTON = R.id.sendbtn;
	private String worldId = null;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		return inflater.inflate(R.layout.fragmen_world, container, false);
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		Activity context = getActivity();
		context.findViewById(R.id.speechbtn).setOnClickListener(clickListenrtImpl);
		context.findViewById(R.id.imagebtn).setOnClickListener(clickListenrtImpl);
		context.findViewById(R.id.sendbtn).setOnClickListener(clickListenrtImpl);
		inputEdit = (EditText)context.findViewById(R.id.inputedit);
		chatRoomView = (ListView)context.findViewById(R.id.list_chatcontent);
		inputRegion = (LinearLayout)context.findViewById(R.id.faceimagepanel);
		if (inputRegion.isShown()){
			inputRegion.setVisibility(View.GONE);
		}
		adapter = new YouMeChatRoomAdapter(context, R.layout.chatmessage_item, chatMessages);
		chatRoomView.setAdapter(adapter);
		super.onActivityCreated(savedInstanceState);
	}
	private ButtonClickListenrtImpl clickListenrtImpl = new ButtonClickListenrtImpl();
	private final class ButtonClickListenrtImpl implements OnClickListener{

		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case SPEECH_BUTTON:
				break;
			case EMOTION_BUTTON:
				break;
			case SEND_BUTTON:
				break;
			default:
				break;
			}
		}

	}
	
	private void sendTextMessage(){
		String message = null;
		if (null != inputEdit)
			message = inputEdit.getText().toString();
		String worldId = YouMeIMManager.getInstance().getWorldId();
		int chatType = YouMeIMManager.CHAT_TYPE_ROOM;
		YouMeIMManager.getInstance().sendTextMessage(worldId, chatType, message, null);
	}
}
