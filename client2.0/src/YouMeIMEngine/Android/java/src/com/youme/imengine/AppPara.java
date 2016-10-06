package com.youme.imengine;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.telephony.TelephonyManager;



@SuppressLint({"DefaultLocale", "NewApi"}) 
public class AppPara 
{
	public static void initPara(Context context)
	{
		try {
			String mPackageNameString = context.getPackageName();
			if (null != mPackageNameString)
			{
				NativeEngine.setPackageName(mPackageNameString);
			}
			
	
			NativeEngine.setModel(Build.MODEL);
			
			NativeEngine.setBrand(Build.BRAND);
				
			NativeEngine.setCPUArch(android.os.Build.CPU_ABI);
			NativeEngine.setCPUChip(android.os.Build.HARDWARE);
			
			
			String mDeviceIMEIString = ((TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE)).getDeviceId();
			if (null != mDeviceIMEIString)
			{
				NativeEngine.setDeviceIMEI(mDeviceIMEIString);
			}
			
			String mSysVersionString = android.os.Build.VERSION.RELEASE;
			if (null != mSysVersionString)
			{
				NativeEngine.setSysVersion(mSysVersionString);
			}	
			PackageManager manager = context.getPackageManager();
			
			//mNetWorkType = NetUtil.getNetworkState(context);
            //AppPara.onNetWorkChange(mNetWorkType);
			
			
			String mDocumentPathString = context.getCacheDir().toString();
			if(null != mDocumentPathString)
			{
				NativeEngine.setDocumentPath(mDocumentPathString);
			}
		} catch (Exception e) {
			// TODO: handle exception
		}
			

	}
		
	public static void onNetWorkChange (int type)
	{
		//mNetWorkType = type;
		NativeEngine.onNetWorkChanged(type);
	}
}
