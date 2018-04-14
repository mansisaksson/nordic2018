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
            transform.position += new Vector3(10f, 0, 0) * Time.deltaTime* Input.GetAxis("Horizontal");
            transform.position += new Vector3(0, 10f, 0) * Time.deltaTime* Input.GetAxis("Vertical");
        
    }
    
}
