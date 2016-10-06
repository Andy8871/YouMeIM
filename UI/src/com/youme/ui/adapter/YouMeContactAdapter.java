package com.youme.ui.adapter;

import java.util.List;

import com.youme.ui.R;
import com.youme.ui.utils.YouMeContact;
import android.R.integer;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.TextView;

public class YouMeContactAdapter extends ArrayAdapter<YouMeContact> {
	private final static String TAG = YouMeContactAdapter.class.getSimpleName();
	private int mResourceId;
	private List<YouMeContact> mContacts;
	public YouMeContactAdapter(Context context, int resource,
			List<YouMeContact> objects) {
		super(context, resource, objects);

		mResourceId = resource;
		mContacts = objects;
	}


	
	
	
	public void selectContact(int position){
		if (!mContacts.get(position).isSelected()){
			mContacts.get(position).setSelected(true);
			for (int i = 0; i < mContacts.size(); ++i){
				if (i != position)
					mContacts.get(i).setSelected(false);
			}
		}
		notifyDataSetChanged();
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		View view = null;
		if (null == convertView)
		{
			view = LayoutInflater.from(getContext()).inflate(mResourceId, null);
			holder = new ViewHolder();
			holder.name = (TextView)view.findViewById(R.id.contact_name);
			holder.head = (ImageView)view.findViewById(R.id.contact_head);
			holder.radioButton = (RadioButton)view.findViewById(R.id.contact_select);
			view.setTag(holder);
		}else {
			view = convertView;
			holder = (ViewHolder)view.getTag();
			
		}
		final int index = position; 
		YouMeContact contact = getItem(position);
		Log.i(TAG, "select item :" + position + ", name:" + contact.getNickName() + ", isSelected:" + contact.isSelected());
		holder.name.setText(contact.getNickName());
		holder.radioButton.setClickable(false);
		holder.radioButton.setChecked(contact.isSelected());
		return view;
	}
	
	
	class ViewHolder{
		private TextView name;	// the name of contact
		
		private ImageView head;	// the head of contact
		
		private RadioButton radioButton;	// select the contact
	}
	

}
