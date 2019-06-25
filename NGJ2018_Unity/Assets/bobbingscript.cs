using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class bobbingscript : MonoBehaviour {

    Vector3 startPoint;
	// Use this for initialization
	void Start () {
        startPoint = transform.position;
	}
	
	// Update is called once per frame
	void Update () {
        transform.position = startPoint + Mathf.Sin(Time.timeSinceLevelLoad*2) * Vector3.up*0.8f;
	}
}
