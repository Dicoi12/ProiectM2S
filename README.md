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
### Modificarea 4
#### Algoritm genetic
**0. Configuratie**
<br>
| Nume |Nume variabila | Valoare|
|----|-----|------|
| Marime Populatie| population_size | 50|
| Numar Gene| gene length|10
| Rata de mutatie| mutation_rate|0.1

**1. Structura individ**
  <br>
Fiecare individ are 2 componente:
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
<br>
Aceasta genereaza o pupulatie de 50 de indivizi cu cate 10 gene formate random cu valori de 1 si 0.
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
<br>
  Se compara similaritatea cu istoricul. Daca in istoric si in individ se regaseste aceiasi valoare atunci fitness-ul individului creste.
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
<br>
Pentru selectia parintilor am ales metoda selectiei prin ruleta
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
<br>
Primul individ care are fitness-ul cumulativ > prag este ales drept parinte
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
<br>
Genereaza un copil din doi parinti. Prima jumatate din primul parinte, iar a doua din al doilea parinte
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
<br>
Pentru fiecare gena a copilului, exita o sansa egala cu *mutation_rate* (10%) ca aceasta sa fie inversata 
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

Aceasta aduce variatie in cadrul populatiei

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

