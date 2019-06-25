﻿using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public class JsonPackage
{
    public float positionX;
    public float positionY;
    public float positionZ;
    public int id;
    public string type;
    public float scaleX;
    public float scaleY;
    public float scaleZ;
    public float angle;
}

[Serializable]
public class JsonPackages
{
    public int packetId;
    public JsonPackage[] jsonPackages;
    public int gravityDirection;

    public JsonPackages(JsonPackage[] packages)
    {
        jsonPackages = packages;
        gravityDirection = Player.gravityId;
    }

}

public class UDPBehaviour : MonoBehaviour {

    public bool isUDPstatic = true;
    public static int IDCOUNTER = 1;
    public bool fromExternalSource = false;
    public int id;
    public string typeName;

    public float timeSinceLastMessage = 0;

    public void Awake() // don't overload this
    {
        id = IDCOUNTER++;
    }

    public void LateUpdate()
    {
        if (fromExternalSource == false) return;
        if(timeSinceLastMessage > 5)
        {
            UDPManager.instance.IdToObject.Remove(this.id);
            Destroy(gameObject);
        }
        timeSinceLastMessage += Time.deltaTime;
    }

    public virtual void Deserialize(JsonPackage message)
    {
        timeSinceLastMessage = 0;
        Vector3 newPos = Vector3.zero;
        newPos.x = message.positionY / 100;
        newPos.y = message.positionZ / 100;
        newPos.z = message.positionX / -100;
        transform.position = newPos;

        if(message.type != "checkpoint")
        {
            Vector3 newScale = Vector3.zero;
            newScale.x = message.scaleY;
            newScale.y = message.scaleZ;
            newScale.z = message.scaleX;
            transform.localScale = newScale;
        }
        id = message.id;

        if (message.type == "Spike")
        {
            transform.eulerAngles = new Vector3(0, 0, message.angle);
           // print(message.angle);
        }
    }

    public JsonPackage Serialize()
    {
        return new JsonPackage()
        {
            positionX = transform.position.x,
            positionY = transform.position.y,
            positionZ = transform.position.z,
            id = id,
            type = typeName,
            scaleX = transform.localScale.x,
            scaleY = transform.localScale.y,
            scaleZ = transform.localScale.z,
            angle = typeName == "Spike" ? transform.localRotation.eulerAngles.z : 0
        };
    }
}
