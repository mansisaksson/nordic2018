using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class StartGame : MonoBehaviour {

    public InputField ipField;
    public InputField portField;

    public void OnStartClick(string level)
    {
        UDPManager.IP = ipField.text.Trim();
        UDPManager.port = System.Convert.ToInt32(portField.text.Trim());
        SceneManager.LoadScene(level);
    }
}
