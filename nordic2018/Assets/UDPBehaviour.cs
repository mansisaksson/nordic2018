using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class JsonPackage
{
    public float positionX;
    public float positionY;
    public string id;
    public string type;
}

[Serializable]
public class JsonPackages
{
    public int packetId;
    public JsonPackage[] jsonPackages;

    public JsonPackages(JsonPackage[] packages)
    {
        jsonPackages = packages;
    }

}

public class UDPBehaviour : MonoBehaviour {

    public Guid guid;

    public void Awake() // don't overload this
    {
        guid = System.Guid.NewGuid();
    }


    public virtual void Deserialize(JsonPackage message)
    {
        Vector3 newPos = Vector3.zero;
        newPos.x = message.positionX;
        newPos.y = message.positionY;
        newPos.z = transform.position.z;
        transform.position = newPos;
    }

    public JsonPackage Serialize()
    {
        return new JsonPackage()
        {
            positionX = transform.position.x,
            positionY = transform.position.y,
            id = guid.ToString(),
            type = GetType().ToString()
        };
    }
}
