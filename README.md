# Proiect
## Tema
 Implementarea unor structuri care să determine gradul de reutilizabilitate al instrucţiunilor (ALU, Load), procentajul de instrucţiuni triviale din benchmark-urile SPEC 2000 (INT / FP). Dezvoltarea unei arhitecturi superscalare şi cu multithreading simultan îmbogăţită cu un mecanism de anticipare selectivă a valorilor instrucţiunilor cu latenţă ridicată de execuţie. Arhitectura propusă include o schemă de reutilizare a rezultatelor instrucţiunilor de înmulţire / împărţire. Cuantificaţi câştigul de performanţă de procesare astfel obţinut. Utilizati algorimti genetici

## Plan
* **Faza 1**: Instructiuni Triviale
  * identificarea instructiunilor triviale
  * contorizare
  * calcularea procentului de instructiuni triviale
  * afisarea contorului si a procentului 
* **Faza 2**: Gradul de reutilizabilitate al instructiunilor ALU si Load
  * identificarea instructiunilor ALU si Load
  * contorizarea intructiunilor ALU si Load (atat numarul total, cat si cel al instructiunilor reutilizate)
  * calcularea gradului de reutilizabilitate
  * afisarea contorului si a procentului de instructiuni reutilizate 
* **Faza 3** Algoritm genetic
    * Stabilirea unei configuratii initiale
    * Structura individului
    * Initializare populatie
    * Calcul fitness
    * Functiile de crossover si mutatie
    * Actualizare populatie 

## Modificari aduse simulatorului Multi2Sim

### Modificare 1
  #### Intructiuni triviale
  **1. Identificarea instructiunilor triviale**
  * Acest lucru se face in cadrul modulului Alu.cc, prin verificarea tipului instructiunii 
  ```cpp
if (type == FunctionalUnit::TypeIntAdd || type == FunctionalUnit::TypeLogic)
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
$\text{Trivial instructions(\%)} = \frac{\text{trivial instructions}}{\text{total instructions}} \times 100$

  **4. Afisarea contorului si a procentului de intructiuni triviale**
* Contor
```cpp
os << misc::fmt("Trivial Instructions = %lld\n", trivial_instructions);
```
* Procentul este calculat cu formula de la punctul 3
```cpp
os << misc::fmt("Trivial Percentage = %.2f%%\n", total_instructions ? (double)trivial_instructions / total_instructions * 100 : 0.0);
```
### Modificarea 2
#### Gradul de reutilizabilitate a intructiunilor ALU si Load
Toate modificarile au fost realizate in cadrul componentei ALU.cc
**1. Identificarea instructiunilor ALU si Load**
```cpp
//Instructiuni ALU
if (type == FunctionalUnit::TypeIntAdd || type == FunctionalUnit::TypeIntMult || type == FunctionalUnit::TypeIntDiv || type == FunctionalUnit::TypeLogic)
 	{
        //incrementam contorul pentru intstructiunile ALU
        if (result_cache.find(uop_id) != result_cache.end())
 			//incrementam contorul pentru instructiunile ALU reutilizate
 	}
```
```cpp
if (type == FunctionalUnit::TypeEffAddr)
 	{
     // incrementam contorul pentru instructiunile Load
     if (result_cache.find(uop_id) != result_cache.end())
        //incrementam contorul pentru instructiunile Load reutilizate
 }
 ```
**2. Contorizare**

Initializam 4 contoare astfel:
1. Contor pentru toate instructiunile ALU
2. Contor pentru instructiunile ALU reutilizate
3. Contor pentru toate instructiunile Load
4. Contor pentru instructiunile Load reutilizate
```cpp
 long long total_alu_instructions = 0;
 long long reused_alu_instructions = 0;
 long long total_load_instructions = 0;
 long long reused_load_instructions = 0;
```
Incrementam contoarele:
```cpp
if (type == FunctionalUnit::TypeIntAdd || 
 		type == FunctionalUnit::TypeIntMult ||
 		type == FunctionalUnit::TypeIntDiv ||
 		type == FunctionalUnit::TypeLogic)
 	{
 		total_alu_instructions++;
 	
 		if (result_cache.find(uop_id) != result_cache.end())
 			reused_alu_instructions++;
 	}

 	auto opcode = uop->getUinst()->getOpcode();
 	if (type == FunctionalUnit::TypeEffAddr)
 	{
     total_load_instructions++;
     if (result_cache.find(uop_id) != result_cache.end())
         reused_load_instructions++;
 }
```
**3. Calcularea gradului de reutilizabilitate**
Gradul de reutilizabilitate a fost calculat cu formulele:
$\text{ALU reused(\%)} = \frac{\text{reused ALU instructions}}{\text{total ALU instructions}} \times 100$

$\text{Load reused(\%)} = \frac{\text{reused Load instructions}}{\text{total Load instructions}} \times 100$

**4. Afisarea relzultatelor**
1. Instructiuni ALU
```cpp
os << misc::fmt("Total ALU Instructions = %lld\n", total_alu_instructions);
 os << misc::fmt("Reused ALU Instructions = %lld\n", reused_alu_instructions);
 os << misc::fmt("ALU Reuse Percentage = %.2f%%\n",
     total_alu_instructions ? (double)reused_alu_instructions / total_alu_instructions * 100 : 0.0);
``` 
2. Instructiuni Load
```cpp
 os << misc::fmt("Total Load Instructions = %lld\n", total_load_instructions);
 os << misc::fmt("Reused Load Instructions = %lld\n", reused_load_instructions);
 os << misc::fmt("Load Reuse Percentage = %.2f%%\n",
     total_load_instructions ? (double)reused_load_instructions / total_load_instructions * 100 : 0.0);
```
### Modificarea 3
#### Algoritm genetic
**0. Configuratie**
| Nume |Nume variabila | Valoare|
|----|-----|------|
| Marime Populatie| population_size | 50|
| Numar Gene| gene_length|10
| Rata de mutatie| mutation_rate|0.1

**1. Structura individ**
* Fiecare individ are 2 componente:
1. Vector de gene cu valori (0 sau 1)
1. Fitness individual
```cpp
struct GeneticIndividual {
    std::vector<int> genes; 
    double fitness;         

    GeneticIndividual(int size) : genes(size), fitness(0.0) {}
};
```
**2. Initializare populatie**
* Aceasta genereaza o pupulatie de 50 de indivizi cu cate 10 gene formate random cu valori de 1 si 0.
```cpp
void InitializePopulation() {
    for (int i = 0; i < population_size; i++) {
        GeneticIndividual individual(gene_length);
        for (int j = 0; j < gene_length; j++) {
            individual.genes[j] = dis(gen); 
        }
        population.push_back(individual);
    }
}
```
**3. Calcul fitness** 
  * Se compara similaritatea cu istoricul. Daca in istoric si in individ se regaseste aceiasi valoare atunci fitness-ul individului creste.
```cpp
void CalculateFitness(const std::vector<int>& history) {
    for (auto& individual : population) {
        individual.fitness = 0.0;
        for (size_t i = 0; i < history.size(); i++) {
            if (i < individual.genes.size() && individual.genes[i] == history[i]) {
                individual.fitness += 1.0;
            }
        }
    }
}
```
**4. Selectie Parinti**
* Pentru selectia parintilor am ales metoda selectiei prin ruleta
1. Calcul fitness total pentru toata populatia 
```cpp
double total_fitness = 0.0;
for (const auto& individual : population) {
    total_fitness += individual.fitness;
}
```

2. Generam un prag random intre 0 si total_fitness
```cpp
double threshold = dis(gen) * total_fitness;
```

3. Iteram prin populatie si acumulam fitness-ul
* Primul individ care are fitness-ul cumulativ > prag este ales drept parinte
```cpp
for (const auto& individual : population) {
    cumulative_fitness += individual.fitness;
    if (cumulative_fitness >= threshold) {
        return individual;
    }
}
```

Aceasta metoda favorizeaza indivizii cu fitness-ul mai mare

4. Crossover
* Genereaza un copil din doi parinti. Prima jumatate din primul parinte, iar a doua din al doilea parinte
```cpp
GeneticIndividual Crossover(const GeneticIndividual& parent1, const GeneticIndividual& parent2) {
    GeneticIndividual offspring(gene_length);
    for (int i = 0; i < gene_length; i++) {
        offspring.genes[i] = (i < gene_length / 2) ? parent1.genes[i] : parent2.genes[i];
    }
    return offspring;
}
```

5. Mutatie
* Pentru fiecare gena a copilului, exita o sansa egala cu *mutation_rate* (10%) ca aceasta sa fie inversata 
```cpp
void Mutate(GeneticIndividual& individual) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < gene_length; i++) {
        if (dis(gen) < mutation_rate) {
            individual.genes[i] = 1 - individual.genes[i];
        }
    }
}
```

* Aceasta aduce variatie in cadrul populatiei

**6. Update population**

* Caluleaza fitness-ul pe toata populatia
* Creaza o populatie noua:
  * Selecteaza 2 parinti
  * Crossover pentru a obtine copil
  * Mutatie pentru diversificare
  * Adauga copilul in populatia noua
* Inlocuieste populatia veche cu cea noua
```cpp
void UpdatePopulation(const std::vector<int>& history) {
    CalculateFitness(history);
    std::vector<GeneticIndividual> new_population;

    for (int i = 0; i < population_size; i++) {
        GeneticIndividual parent1 = SelectParent();
        GeneticIndividual parent2 = SelectParent();
        GeneticIndividual offspring = Crossover(parent1, parent2);
        Mutate(offspring);
        new_population.push_back(offspring);
    }

    population = new_population;
}
```

**7. Predictie Valoare**
1. Alege individul cu fitness-ul cel mai mare
2. Returneaza prima sa gena
> Se presupune ca prima gena ar fii prima valoare de dupa ce se termina istoricul
```cpp
int PredictValue(const std::vector<int>& history) {
    CalculateFitness(history);
    auto best_individual = std::max_element(population.begin(), population.end(),[](const GeneticIndividual& a, const GeneticIndividual& b) {
            return a.fitness < b.fitness;
        });

    return best_individual->genes[0];
}
```

