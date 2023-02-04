using System;
using System.Dynamic;
using System.Runtime.InteropServices;
using UnityEngine;


public class NEAT : MonoBehaviour
{
    public enum ModeSelection
    {
        Learning,
        ApplyTrained,
    };

    [SerializeField] private ModeSelection typeApprentisage;
    
    private enum BestRotationDirection
    {
        Right,
        Forward,
        Left
    }

    private BestRotationDirection _directionFromNeat;
    
    private IntPtr _parameterInstance = IntPtr.Zero;
    private IntPtr _neatInstance = IntPtr.Zero;
    private IntPtr _goatGenomeInstance = IntPtr.Zero;

    private Vector3 _directionRight;
    private Vector3 _directionForward;
    private Vector3 _directionLeft;
    
    private Vector3 _defaultPosition;
    private Quaternion _defaultRotation;
    
    private bool _hasIsAWall;
    private Rigidbody _rigidbody;

    [SerializeField] private float range = 3;
    [SerializeField] private int popSize = 25;

    public GameObject checkPointPassed = null;

    private string _nameFileToSave;
    
    private void Start()
    {
        _hasIsAWall = true;
        _rigidbody = GetComponent<Rigidbody>();
        _defaultPosition = transform.position;
        _defaultRotation = _rigidbody.rotation;

        _directionFromNeat = BestRotationDirection.Right;
        
        _directionRight = new Vector3(1, 0, 1);
        _directionForward = Vector3.forward;
        _directionLeft = new Vector3(-1, 0, 1);
        
        _nameFileToSave = "test.txt";
        
        _parameterInstance = InitParameterInstance();
        
        if (_parameterInstance == null)
            throw new InvalidOperationException("param instance is not init");
        
        _neatInstance = InstantiateNeat(popSize, 4, 3, _parameterInstance);

        if (_neatInstance == null)
            throw new InvalidOperationException("neat is not init");

        if (typeApprentisage == ModeSelection.ApplyTrained)
        {
            _goatGenomeInstance = LoadGoat(_nameFileToSave);
            /*NetworkFromGenome(_goatGenomeInstance);*/
        }
    }

    private void OnDestroy()
    {
        DeleteInstance(_neatInstance);
        DeleteInstance(_parameterInstance);
        DeleteInstance(_goatGenomeInstance);
    }
    
    void Update()
    {
        float distanceRightRayCast = RayCast(_directionRight);
        float distanceForwardRayCast = RayCast(_directionForward);
        float distanceLeftRayCast = RayCast(_directionLeft);

        Evaluate(new float[] { distanceRightRayCast, distanceForwardRayCast, distanceLeftRayCast});

        if (Input.GetKeyDown(KeyCode.S))
        {
            SaveNeatGoat();
        }

    }
    void FixedUpdate()
    {
        if (_hasIsAWall)
        {
            _rigidbody.MoveRotation(_defaultRotation);
            _rigidbody.MovePosition(_defaultPosition);
            _hasIsAWall = false;
            return;
        }

        Quaternion deltaRotation = Quaternion.Euler(new Vector3(0, 0, 0) * Time.fixedDeltaTime);
        
        switch (_directionFromNeat)
        {
            case BestRotationDirection.Left :
                deltaRotation = Quaternion.Euler(new Vector3(0, -100, 0) * Time.fixedDeltaTime);
                break;
            case BestRotationDirection.Right :
                deltaRotation = Quaternion.Euler(new Vector3(0, 100, 0) * Time.fixedDeltaTime);
                break;
        }

        _rigidbody.MoveRotation(_rigidbody.rotation * deltaRotation);
        _rigidbody.MovePosition(transform.position + transform.forward * Time.deltaTime * 5.0f);
    }
    
    private void SaveNeatGoat()
    {
        SaveGoat(_neatInstance, _nameFileToSave);
    }
    
    private void Evaluate(float[] rayCastValues)
    {
        float[] sumOutputsFromNeuralNetwork = new float[3];
        
        for (int epoque = 0; epoque < 10; epoque++)
        {
            float[] adjustePopFitness = new float[popSize];
            float[] outputsFromNeuralNetwork = new float[3];
            
            for (int i = 0; i < popSize; i++)
            {
                IntPtr neuralNetwork = GetNeuralNetwork(_neatInstance, i);
                GetNeuralNetworkCompute(neuralNetwork, outputsFromNeuralNetwork,3);

                for (int j = 0; j < 3; j++)
                {
                    sumOutputsFromNeuralNetwork[j] = outputsFromNeuralNetwork[j]; 
                    adjustePopFitness[i] += outputsFromNeuralNetwork[j];
                    adjustePopFitness[i] += rayCastValues[j];
                }
                
                if (checkPointPassed != null)
                {
                    adjustePopFitness[i] += checkPointPassed.GetComponent<CheckPoint>().GetWeight();
                }
                
            }

            SetScore(_neatInstance, adjustePopFitness, (uint) popSize);
            /*Evolve(_neatInstance);*/
        }

        float bestMove = 0;
        
        for (int i = 0; i < 3; i++)
        {
            if (bestMove > sumOutputsFromNeuralNetwork[i])
                continue;
            
            switch (i)
            {
                case 0 :
                    _directionFromNeat = BestRotationDirection.Left;
                    return;
                case 1 :
                    _directionFromNeat = BestRotationDirection.Forward;
                    return;
                case 2 :
                    _directionFromNeat = BestRotationDirection.Right;
                    return;
            }
            bestMove = sumOutputsFromNeuralNetwork[i];
        }
        
    }

    private float RayCast(Vector3 direction)
    {
        Ray theRay = new Ray(transform.position, transform.TransformDirection(direction * range));
        Debug.DrawRay(transform.position, transform.TransformDirection(direction * range));

        if (Physics.Raycast(theRay, out RaycastHit hit, range))
        {
            if (hit.collider.CompareTag("Wall"))
            {
                return 1 - (hit.distance / range);
            }
        }

        return 1;
    }
    
    void OnTriggerEnter(Collider other) 
    {
        if(other.gameObject.CompareTag("Wall"))
        {
            _hasIsAWall = true;
        }
        else
        {
            _hasIsAWall = false;
        }
    }
    
    [DllImport("NEAT.dll")]
    public static extern IntPtr GetNeuralNetwork(IntPtr neat, int idx);

    [DllImport("NEAT.dll")]
    public static extern void GetNeuralNetworkCompute(IntPtr neat, [In, Out] float[] resultArray, int legnth);
    
    [DllImport("NEAT.dll")]
    public static extern IntPtr InitParameterInstance();
    
    [DllImport("NEAT.dll")]
    public static extern void DeleteInstance(IntPtr instance);
    
    [DllImport("NEAT.dll")]
    public static extern IntPtr InstantiateNeat(int popSize, uint input, uint output, IntPtr neatParam);
    
    [DllImport("NEAT.dll")]
    public static extern void SetScore(IntPtr neat, float[] score, uint length);

    [DllImport("NEAT.dll")]
    public static extern void SaveGoat(IntPtr neat, string fileName);
    
    [DllImport("NEAT.dll")]
    public static extern void Evolve(IntPtr neat);

    [DllImport("NEAT.dll")]
    public static extern IntPtr LoadGoat(string fileName);
    
    [DllImport("NEAT.dll")]
    public static extern void NetworkFromGenome(IntPtr genome, IntPtr neuralNetwork);
}
