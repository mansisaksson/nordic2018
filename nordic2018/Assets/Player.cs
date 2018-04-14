using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Player : UDPBehaviour
{
    public Rigidbody rb;
    public float gravityDirection = 1;

    public Vector3 startPos;

    // Use this for initialization
    void Start () {
        startPos = transform.position;
        rb = GetComponent<Rigidbody>();
        Physics.gravity = new Vector3(0, 0, 0);
    }
	
	// Update is called once per frame
	void Update () {
        if (fromExternalSource == false)
        {

            // movement
            var x = Input.GetAxis("Horizontal") * Time.deltaTime * 10.0f;
            rb.MovePosition(transform.position + new Vector3(x, 0f, 0f));


            //invert gravity, only if grounded
            if (Input.GetKeyDown("space") && IsGrounded())
                gravityDirection *= -1;
        }
    }

    private void FixedUpdate()
    {
        if (fromExternalSource == false)
        {

            rb.AddForce(0f, (Time.deltaTime * 1000.0f * gravityDirection), 0f);
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.name.Contains("Spike"))
        {
            //Debug.Log("collided with \"spike\"");
            //SceneManager.LoadScene(SceneManager.GetActiveScene().name);
            transform.position = startPos;
        }
    }

    private bool IsGrounded()
   {
        return Physics.Raycast(transform.position, Vector3.up * gravityDirection, 0.501f);
   }
}
