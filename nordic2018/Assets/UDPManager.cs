using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Linq;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
public class UDPManager : MonoBehaviour {
    public List<UDPBehaviour> prefabs = new List<UDPBehaviour>();
    Dictionary<string, UDPBehaviour> TypeToPrefab = new Dictionary<string, UDPBehaviour>();
    

    Dictionary<int, UDPBehaviour> IdToObject = new Dictionary<int, UDPBehaviour>();
    //Dictionary<UDPBehaviour, string> ObjectToId = new Dictionary<UDPBehaviour, string>();

    // start from unity3d
    public void Start()
    {
        foreach(UDPBehaviour obj in prefabs)
        {
            TypeToPrefab.Add(obj.name, obj);
        }
        _udpManager = this;
        UDPBehaviour[] objects = FindObjectsOfType<UDPBehaviour>();
        for (int i = 0; i < objects.Length; i++)
        {
            IdToObject.Add(objects[i].id, objects[i]);
           // ObjectToId.Add(objects[i], objects[i].guid.ToString());
            print("id added " + objects[i].id.ToString());
        }

        initUDP();
    }

    void Update()
    {
        // receive
        string message = Interlocked.Exchange(ref messageReceived, null);
        if(message != null)
        {
            DeserializeJsonMessage(message);
        }

        // send

        List<UDPBehaviour> objects = new List<UDPBehaviour>(FindObjectsOfType<UDPBehaviour>());
        objects.RemoveAll(x => x.fromExternalSource == true);

        JsonPackage[] packages = new JsonPackage[objects.Count];

        for (int i = 0; i < packages.Length; i++)
        {
            packages[i] = objects[i].Serialize();
        }

        sendString(JsonUtility.ToJson(new JsonPackages(packages)));
    }

    void FixedUpdate()
    {
       
    }

    void DeserializeJsonMessage(string message)
    {
        JsonPackage[] packages = JsonUtility.FromJson<JsonPackages>(message).jsonPackages;
        for (int i = 0; i < packages.Length; i++)
        {
            UDPBehaviour obj;
            IdToObject.TryGetValue(packages[i].id, out obj);

            if(obj != null)
            {
                // update
                obj.Deserialize(packages[i]);
            }
            else
            {
                // create
                UDPBehaviour newObj = null;
                if(TypeToPrefab.TryGetValue(packages[i].type, out newObj))
                {
                    newObj = Instantiate(newObj);
                    newObj.fromExternalSource = true;
                    IdToObject.Add(packages[i].id, newObj);
                    newObj.Deserialize(packages[i]);
                }
                else
                {
                    Debug.LogError("can't find prefab: " + packages[i].type);
                }
            }
        }
    }
/*
 
    -----------------------
    UDP-Receive (send to)
    -----------------------
    // [url]http://msdn.microsoft.com/de-de/library/bb979228.aspx#ID0E3BAC[/url]
   
   
    // > receive
    // 127.0.0.1 : 8051
   
    // send
    // nc -u 127.0.0.1 8051
 
*/

    // receiving Thread
    Thread receiveThread;

    // udpclient object
    UdpClient receiverClient;
    UdpClient senderClient;
    // public
    // public string IP = "127.0.0.1"; default local
    [SerializeField]
    public int port = 8051; // define > init

    private static UDPManager _udpManager;
    public static UDPManager instance
    {
        get
        {
            if (_udpManager == null)
                _udpManager = FindObjectOfType<UDPManager>();
            return _udpManager;
        }
    }

 

    // OnGUI
 /*   void OnGUI()
    {
        Rect rectObj = new Rect(40, 10, 200, 400);
        GUIStyle style = new GUIStyle();
        style.alignment = TextAnchor.UpperLeft;
        GUI.Box(rectObj, "# UDPReceive\n127.0.0.1 " + port + " #\n"
                    + "shell> nc -u 127.0.0.1 : " + port + " \n"
                    + "\nLast Packet: \n" + lastReceivedUDPPacket
                    + "\n\nAll Messages: \n" + allReceivedUDPPackets
                , style);

        rectObj = new Rect(40, 380, 200, 400);
        style = new GUIStyle();
        style.alignment = TextAnchor.UpperLeft;
        GUI.Box(rectObj, "# UDPSend-Data\n127.0.0.1 " + port + " #\n"
                    + "shell> nc -lu 127.0.0.1  " + port + " \n"
                , style);

        // ------------------------
        // send it
        // ------------------------
        strMessage = GUI.TextField(new Rect(40, 420, 140, 20), strMessage);
        if (GUI.Button(new Rect(190, 420, 40, 20), "send"))
        {
            sendString(strMessage + "\n");
        }
    }*/

    // init
    private void initUDP()
    {
        // Endpunkt definieren, von dem die Nachrichten gesendet werden.
        print("UDPSend.init()");
        

        // status
        print("Sending to 127.0.0.1 : " + port);
        print("Test-Sending to this Port: nc -u 127.0.0.1  " + port + "");


        // ----------------------------
        // Abhören
        // ----------------------------
        // Lokalen Endpunkt definieren (wo Nachrichten empfangen werden).
        // Einen neuen Thread für den Empfang eingehender Nachrichten erstellen.
        receiveThread = new Thread(
            new ThreadStart(ReceiveData));
        receiveThread.IsBackground = true;
        receiveThread.Start();





        // Endpunkt definieren, von dem die Nachrichten gesendet werden.
        print("UDPSend.init()");
        

        // ----------------------------
        // Senden
        // ----------------------------
        remoteEndPoint = new IPEndPoint(IPAddress.Parse(IP), port);
        senderClient = new UdpClient();

        // status
        print("Sending to " + IP + " : " + port);
        print("Testing: nc -lu " + IP + " : " + port);


    }

    void OnDisable()
    {
        if (receiveThread != null)
            receiveThread.Abort();

        receiverClient.Close();
    }

    // receive thread
    private void ReceiveData()
    {

        receiverClient = new UdpClient(port);
        while (true)
        {

            try
            {
                // Bytes empfangen.
                IPEndPoint anyIP = new IPEndPoint(IPAddress.Any, port);
                byte[] data = receiverClient.Receive(ref anyIP);

                // Bytes mit der UTF8-Kodierung in das Textformat kodieren.
                string text = Encoding.UTF8.GetString(data);
                messageReceived = text;
                //DeserializeJsonMessage(text);

                // Den abgerufenen Text anzeigen.
                print(">> " + text);

                // latest UDPpacket
             //   lastReceivedUDPPacket = text;

                // ....
             //   allReceivedUDPPackets = allReceivedUDPPackets + text;

            }
            catch (Exception err)
            {
                print(err.ToString());
            }
        }
    }
    private string messageReceived = null;


    ////////////////////////////////////////


    private static int localPort;

    // prefs
    [SerializeField]
    private string IP = "127.0.0.1";  // define in init

    // "connection" things
    IPEndPoint remoteEndPoint;

    // sendData
    public void sendString(string message)
    {
        try
        {
            //if (message != "")
            //{

            // Daten mit der UTF8-Kodierung in das Binärformat kodieren.
            byte[] data = Encoding.UTF8.GetBytes(message);

            // Den message zum Remote-Client senden.
            senderClient.Send(data, data.Length, remoteEndPoint);
            //}
        }
        catch (Exception err)
        {
            print(err.ToString());
        }
    }

}
 
