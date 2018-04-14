using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Lever : UDPBehaviour
{

    public UDPBehaviour WallToMove;

    // Use this for initialization
    void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.name.Contains("MainCharacter"))
        {
            Debug.Log("collided with \"lever\"");
            WallToMove.transform.position = new Vector3(5000, 5000, 0);
        }
    }
}
