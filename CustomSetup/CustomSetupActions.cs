using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.IO;

// Reference: https://www.c-sharpcorner.com/article/how-to-perform-custom-actions-and-upgrade-using-visual-studio-installer/
namespace SetupCustomActions
{
    [RunInstaller(true)]
    public partial class CustomActions : System.Configuration.Install.Installer
    {
        public CustomActions()
        {
        }

        protected override void OnAfterInstall(IDictionary savedState)
        {
            var installPath = Path.GetDirectoryName(base.Context.Parameters["AssemblyPath"]);

            // We want to add our layer at the very beginning, so that any other layer like the OpenXR Toolkit layer is following us.
            // We delete all entries, create our own, and recreate all entries.

            Microsoft.Win32.RegistryKey key;
            {
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit");
                var jsonName = "XR_APILAYER_MBUCCHIA_vulkan_d3d12_interop.json";
                var jsonPath = installPath + "\\" + jsonName;

                ReOrderApiLayers(key, jsonName, jsonPath);

                key.Close();
            }
            {
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("SOFTWARE\\WOW6432Node\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit");
                var jsonName = "XR_APILAYER_MBUCCHIA_vulkan_d3d12_interop-32.json";
                var jsonPath = installPath + "\\" + jsonName;

                ReOrderApiLayers(key, jsonName, jsonPath);

                key.Close();
            }

            base.OnAfterInstall(savedState);
        }

        private void ReOrderApiLayers(Microsoft.Win32.RegistryKey key, string jsonName, string jsonPath)
        {
            var existingValues = key.GetValueNames();
            var entriesValues = new Dictionary<string, object>();
            foreach (var value in existingValues)
            {
                entriesValues.Add(value, key.GetValue(value));
                key.DeleteValue(value);
            }

            key.SetValue(jsonPath, 0);
            foreach (var value in existingValues)
            {
                // Do not re-create our own key. We did it before this loop.
                if (value.EndsWith("\\" + jsonName))
                {
                    continue;
                }

                key.SetValue(value, entriesValues[value]);
            }
        }
    }
}
