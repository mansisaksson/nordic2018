using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class JsonPackage
{
    public float positionX;
    public float positionY;
    public float positionZ;
    public string id;
    public string type;
    public float scaleX;
    public float scaleY;
    public float scaleZ;
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

    public bool fromExternalSource = false;
    public Guid guid;

    public void Awake() // don't overload this
    {
        guid = System.Guid.NewGuid();
    }


    public virtual void Deserialize(JsonPackage message)
    {
        Vector3 newPos = Vector3.zero;
        newPos.x = message.positionY / 100;
        newPos.y = message.positionZ / 100;
        newPos.z = message.positionX / -100;
        transform.position = newPos;

        Vector3 newScale = Vector3.zero;
        newScale.x = message.scaleY;
        newScale.y = message.scaleZ;
        newScale.z = message.scaleX;
        transform.localScale = newScale;
    }

    public JsonPackage Serialize()
    {
        return new JsonPackage()
        {
            positionX = transform.position.x,
            positionY = transform.position.y,
            positionZ = transform.position.z,
            id = guid.ToString(),
            type = gameObject.name,
            scaleX = transform.localScale.x,
            scaleY = transform.localScale.y,
            scaleZ = transform.localScale.z,
        };
    }
}
