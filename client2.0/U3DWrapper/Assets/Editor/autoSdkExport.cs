using UnityEngine;
using UnityEditor;

using System.Collections;

public class AutoSdkExport:Editor {

  //[MenuItem("Automation/Hello")]
  static int AutoExport() {
    string[] args = System.Environment.GetCommandLineArgs();
	
    string methodName = "AutoSdkExport.AutoExport";
    bool start = false;

	ArrayList sourcePath = new ArrayList();
    for (int i = 0; i < args.Length; i++)  
    {
	    if (start == true)
		{
			//Debug.Log(args[i]);
			sourcePath.Add(args[i]);
		}
		
		if (string.Compare(args[i], methodName) == 0)
		{
			start = true;
		}
    }

	if (sourcePath.Count <= 1)
	{
		Debug.Log("miss source path");
		return 1;
	}
	
	string exportFile = (string)sourcePath[sourcePath.Count - 1];
	
	sourcePath.RemoveAt(sourcePath.Count - 1);
	string[] assetPathNames = (string[])sourcePath.ToArray(typeof(string));
	
	AssetDatabase.ExportPackage(assetPathNames, exportFile, ExportPackageOptions.IncludeDependencies | ExportPackageOptions.Recurse);
	
	return 0;
  }
}