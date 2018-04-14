using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : UDPBehaviour
{
    public Rigidbody rb;
    public float direction = 1;

    // Use this for initialization
    void Start () {
        rb = GetComponent<Rigidbody>();
        Physics.gravity = new Vector3(0, 0, 0);
    }
	
	// Update is called once per frame
	void Update () {
        var x = Input.GetAxis("Horizontal") * Time.deltaTime * 10.0f;

        //transform.Translate(x, 0, 0);
        rb.MovePosition(transform.position + new Vector3(x, 0f, 0f));

        if (Input.GetKeyDown("space"))
            direction *= -1;
    }

    private void FixedUpdate()
    {
        rb.AddForce(0f, (Time.deltaTime * 500.0f * direction), 0f);
    }
}
