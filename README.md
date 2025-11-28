# 2. Task Selection

## Task: Computing the n-th Derivative of a Function Using Numerical Differentiation

### Mathematical Problem Statement
For a given function \( f(x) \) and integer \( n \geq 1 \), compute the value of the n-th derivative at point \( x_0 \):
\[
f^{(n)}(x_0) = \frac{d^n f}{dx^n}\bigg|_{x=x_0}
\]

### Numerical Method
We use the second-order central difference formula:
\[
f'(x) \approx \frac{f(x+h) - f(x-h)}{2h}
\]
where \( h \) is a small differentiation step.

For higher-order derivatives, we apply the recursive scheme:
\[
f^{(k)}(x) \approx \frac{f^{(k-1)}(x+h) - f^{(k-1)}(x-h)}{2h}
\]

### Example
For the function \( f(x) = x^3 + 2x^2 + 3x + 4 \):
- \( f'(x) = 3x^2 + 4x + 3 \)
- \( f''(x) = 6x + 4 \)
- \( f'''(x) = 6 \)
- \( f^{(4)}(x) = 0 \)

At \( x = 1.0 \):
- \( f'(1.0) = 3(1)^2 + 4(1) + 3 = 10.0 \)
- \( f''(1.0) = 6(1) + 4 = 10.0 \)
- \( f'''(1.0) = 6.0 \)

### Why This Task is Ideal for the Pipeline Pattern

#### 1. Natural Sequential Dependencies
- To compute the 2nd derivative, you first need the 1st derivative
- To compute the 3rd derivative, you first need the 2nd derivative
- Each computation step depends directly on the previous result

#### 2. Perfect Process Distribution
```
Process 0: f(x) → f'(x)     (1st derivative)
Process 1: f'(x) → f''(x)   (2nd derivative) 
Process 2: f''(x) → f'''(x) (3rd derivative)
...
Process k: f⁽ᵏ⁾(x) → f⁽ᵏ⁺¹⁾(x) ((k+1)-th derivative)
```

#### 3. Minimal Data Transfer
- Only the current derivative value needs to be passed between processes
- Each process receives input, performs computation, and sends output
- Perfect alignment with the pipeline data flow model

#### 4. Clear Demonstration of Pipeline Benefits
- **Specialization**: Each process handles exactly one differentiation step
- **Sequential Processing**: Natural flow from lower to higher derivatives
- **Scalability**: Easy to increase derivative order by adding more processes
- **Overlap Potential**: Multiple x-values could be processed concurrently in a real scenario

#### 5. Educational Value
- Clearly demonstrates how complex computations can be broken into sequential stages
- Shows the power of dividing work where each stage depends on the previous
- Illustrates both the strengths and limitations of pipeline parallelism

### Implementation Specifics for Student 61
- **Data Type**: `float` (as calculated from student number)
- **Function**: \( f(x) = x^3 + 2x^2 + 3x + 4 \)
- **Test Point**: \( x = 1.0 \)
- **Step Size**: \( h = 0.0001 \)

This task provides an excellent demonstration of the pipeline pattern while solving a meaningful mathematical problem with clear real-world applications in scientific computing and numerical analysis.