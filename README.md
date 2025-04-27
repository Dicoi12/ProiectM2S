# Proiect
## Tema
 Implementarea unor structuri care să determine gradul de reutilizabilitate al instrucţiunilor (ALU, Load), procentajul de instrucţiuni triviale din benchmark-urile SPEC 2000 (INT / FP). Dezvoltarea unei arhitecturi superscalare şi cu multithreading simultan îmbogăţită cu un mecanism de anticipare selectivă a valorilor instrucţiunilor cu latenţă ridicată de execuţie. Arhitectura propusă include o schemă de reutilizare a rezultatelor instrucţiunilor de înmulţire / împărţire. Cuantificaţi câştigul de performanţă de procesare astfel obţinut. Utilizati algorimti genetici

## Plan
* Faza 1: Instructiuni Triviale
  * identificarea instructiunilor triviale
  * contorizarea intructiunilor triviale
  * calcularea procentului de instructuini triviale
  * afisarea contorului si a procentului 
* Faza 2: 
  *  

## Modificari aduse simulatorului Multi2Sim

### Modificare 1
  #### Intructiuni triviale
  **1. Identificarea instructiunilor triviale**
  * Acest lucru se face in cadrul modulului Alu.cc, prin verificarea tipului instructiunii 
  ```cpp
if (type == FunctionalUnit::TypeIntAdd)
 	{
 	// incrementam un contor	
 	}
```
  **2. Contorizarea instructiunilor triviale**
  * Initializam o variabila pentru contorizarea instructiunilor triviale, in cadrul componentei Alu.cc
```cpp
    long long trivial_instructions = 0;  
```
  * Incrementam contorul daca instructiunea e triviala
  ```cpp
if (type == FunctionalUnit::TypeIntAdd)
 	{
 		trivial_instructions++;
 	}
```
 **3. Calcularea procentrului de instructiuni triviale** 
 * Formula
 </br>
 </br>
  $`Trivial_{}= \frac {trivial-{instructions}}{total-{instructions}}*100`$

  **4. Afisarea contorului si a procentului de intructiuni triviale**
* Contor
```cpp
os << misc::fmt("Trivial Instructions = %lld\n", trivial_instructions);
```
* Procentul este calculat cu formula de la punctul 3
```cpp
os << misc::fmt("Trivial Percentage = %.2f%%\n", total_instructions ? (double)trivial_instructions / total_instructions * 100 : 0.0);
```
