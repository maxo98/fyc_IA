using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CheckPoint : MonoBehaviour
{
    [SerializeField] private GameObject previousCheckPoint = null;
    [SerializeField] private GameObject currentCheckPoint;
    [SerializeField] private GameObject entity;
    [SerializeField] private float weight;
    
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.CompareTag("Player"))
        {
            UpdateEntityCheckPointPassed();
        }
    }

    public void UpdateEntityCheckPointPassed()
    {
        if (entity.GetComponent<NEAT>().checkPointPassed == previousCheckPoint)
        {
            entity.GetComponent<NEAT>().checkPointPassed = currentCheckPoint;
        }
    }

    public float GetWeight()
    {
        return weight;
    }
}
