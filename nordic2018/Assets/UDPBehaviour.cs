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
    public JsonPackages(JsonPackage[] packages)
    {
        jsonPackages = packages;
    }

    public JsonPackage[] jsonPackages;
}

public class UDPBehaviour : MonoBehaviour {

    public Guid guid;

    public void Awake() // don't overload this
    {
        guid = System.Guid.NewGuid();
    }

    public void Deserialize(JsonPackage message)
    {

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
