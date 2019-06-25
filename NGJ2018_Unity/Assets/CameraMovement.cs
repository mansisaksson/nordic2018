using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraMovement : MonoBehaviour {

    public Player player;
    public float dampTime = 0.1f;

    private Vector3 moveVelocity;

    // Use this for initialization
    void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {

    }

    private void LateUpdate()
    {
        if (player != null)
        {
            transform.position = (Vector3.SmoothDamp(transform.position, new Vector3(player.transform.position.x, player.transform.position.y, transform.position.z), ref moveVelocity, dampTime * Time.smoothDeltaTime));
        }
    }
}
