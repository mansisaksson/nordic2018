using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class testUDP : UDPBehaviour
{

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update ()
    {
        if(fromExternalSource == false)
        {

            transform.position += new Vector3(10f, 0, 0) * Time.deltaTime* Input.GetAxis("Horizontal");
            transform.position += new Vector3(0, 10f, 0) * Time.deltaTime* Input.GetAxis("Vertical");

           /* var fakeMimicObj = Serialize();
            fakeMimicObj.id = "1234512345";
            fakeMimicObj.positionX += 4;
            fakeMimicObj.positionY -= 3;
            JsonPackage[] packages = new JsonPackage[] { fakeMimicObj };
            UDPManager.instance.sendString(JsonUtility.ToJson(new JsonPackages(packages)));*/
        }
    }
    
}
