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
* **Faza 3**: Value Predictor
  * crearea unui Prediction Table
  * predictie
  * update
* **Faza 4** Algoritm genetic
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
if (type == FunctionalUnit::TypeIntAdd || type == FunctionalUnit::TypeLogic))
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
#### Value Prediction
**1. Prediction Table**
* Este un unordered_map, cu chei ce reprezinta id-ul instructiunilor, iar valoarea este ValueHistory
* ValueHistory:
  * values: lista ultimelor valori rezultate
  * stride: diferenta dintre valori
  * last_prediction: ultima predictie facuta
  * confidence: increderea in predictie
  * has_prediction: flag daca exista o predictie recenta

**2. Predictie**
* Cautam in istoric pentru id-ul instructiunii
* Daca nu avem cel putin 2 valori nu putem prezice
* Verificam daca stride-ul este constant:
  * da: urmatoarea valoare = ultima valoare + stride
  * nu: folosim ultima valoare 
* Incrementam contoarele:
  * total_predictions
  * confident_predictions
```cpp
bool ValuePredictor::predict(Uop* uop, long long& predicted_value)
{
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;  
        
    ValueHistory& history = it->second;
    
    if (history.values.size() < 2)
        return false;  
        
    bool has_constant_stride = true;
    int stride = history.values[1] - history.values[0];
    
    for (size_t i = 2; i < history.values.size(); i++)
    {
        if (history.values[i] - history.values[i-1] != stride)
        {
            has_constant_stride = false;
            break;
        }
    }
    
    if (has_constant_stride)
    {
        predicted_value = history.values.back() + stride;
        history.stride = stride;
    }
    else
    {
        predicted_value = history.values.back();
    }

    total_predictions++;
    if (isConfidentPrediction(uop)) {
        confident_predictions++;
    }
    
    history.last_prediction = predicted_value;
    history.has_prediction = true;
    
    return true;
}
```
**3. Update**
* Verificam daca a fost o predictie anterioara:
  * da: comparam cu valoarea reala:
    * da: correct_predictions++
* Salvam noua valoare in istoric
* Daca stride-ul dintre ultimele doua valori este constant marim confidence-ul
```cpp
void ValuePredictor::update(Uop* uop, long long actual_value)
{
    long long uop_id = uop->getId();
    ValueHistory& history = prediction_table[uop_id];
    if (history.has_prediction) {
        if (history.last_prediction == actual_value) {
            correct_predictions++;
            if (isConfidentPrediction(uop)) {
                correct_confident_predictions++;
            }
        }
        history.has_prediction = false;
    }
    history.values.push_back(actual_value);
    if (history.values.size() > HISTORY_SIZE)
        history.values.erase(history.values.begin());
    if (history.values.size() >= 2)
    {
        int current_stride = history.values.back() - history.values[history.values.size()-2];
        if (current_stride == history.stride)
            history.confidence++;
        else
            history.confidence = 0;
    }
}
```

### Modificarea 4
#### Algoritm genetic
**0. Configuratie**
| Nume |Nume variabila | Valoare|
|----|-----|------|
|marime populatie|population_size|100
|generatii|generations|50
|rata de mutatie |mutation_rate|0.1
|rata de crossover|crossover_rate|0.7
|marime selectie (tournament)|tournament_size|5

**1. Structura individ**
* Fiecare individ are 2 componente:
1. Vector de gene:
  * history_size: pozitia 0
  * confidence_threshold: pozitia 1
  * stride_window: pozitia 2
  * reuse_treshold: pozitia 3

2. Fitness individual
```cpp
struct GeneticIndividual {
    std::vector<int> genes; 
    double fitness;         

    GeneticIndividual(int size) : genes(size), fitness(0.0) {}
};
```
**2. Initializare populatie**
* Aceasta genereaza o populatie de 100 de indivizi cu gene random.
* Genele au urmatoarele intervale
  * history_size: 1 - 10 (cate valori memoram pentru fiecare instructiune)
  * confidence_threshold: 1 - 5 (cate potriviri de stride pentru a fii confident)
  * stride_window: 1 - 5 (cate diferente consecutive verificam)
  * reuse_treshold: 0% - 99% (pragul pentru cat de reutilizabile sunt valorile)
* Initializeaza fintess-ul tuturor indivizilor la 0
```cpp
void GeneticValuePredictor::initializePopulation() {
    population.resize(params.population_size);
    std::uniform_int_distribution<int> history_dist(1, 10);
    std::uniform_int_distribution<int> confidence_dist(1, 5);
    std::uniform_int_distribution<int> stride_dist(1, 5);
    std::uniform_int_distribution<int> reuse_dist(0, 99);

    for (auto& chromosome : population) {
        chromosome.genes.resize(4);
        chromosome.genes[0] = history_dist(rng);    
        chromosome.genes[1] = confidence_dist(rng); 
        chromosome.genes[2] = stride_dist(rng);    
        chromosome.genes[3] = reuse_dist(rng);    
        chromosome.fitness = 0.0;
    }
}
```
**3. Calcul fitness** 
  * Evalueaza cat de bune sunt predictiile din prediction_table pe baza valorilor existente
  * Daca utima valoare prezisa (last_prediction) este egala cu ultima valoare reala, atunci predictia este corecta.
  Returneaza procentul de predictii corecte
```cpp
double GeneticValuePredictor::evaluateFitness(const Chromosome& chromosome) {
    PredictorParams params = decodeChromosome(chromosome);
    double correct = 0;
    double total = 0;
    for (const auto& entry : prediction_table) {
        if (entry.second.has_prediction) {
            total++;
            if (entry.second.last_prediction == entry.second.values.back()) {
                correct++;
            }
        }
    }
    return total > 0 ? correct / total : 0;
}
```
**4. Selectie Parinti**
* Pentru selectie, am ales metoda turnamenului. Adica se face o selectie de 5 indivizi (tournament_size) dintre care se aleg parintii cei cu fitness-ul cel mai mare.
* Pentru a favoriza elitismul, luam si individul cu fitness-ul cel mai mare
```cpp
void GeneticValuePredictor::selection() {
    std::vector<Chromosome> new_population;
    new_population.reserve(params.population_size);

    auto best = std::max_element(population.begin(), population.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });
    new_population.push_back(*best);

    while (new_population.size() < params.population_size) {
        new_population.push_back(tournamentSelection());
    }

    population = std::move(new_population);
}
```
**5. Crossover**
* Genereaza un copil din doi parinti.
* Pentru fiecare 2 indivizi consecutivi:
  * se alege un punct aleator in gene
  * inainte de acel punct se iau genele primului parinte, iar dupa se iau genele celui de-al doilea
```cpp
void GeneticValuePredictor::crossover() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> point_dist(0, 3);

    for (size_t i = 1; i < population.size(); i += 2) {
        if (dist(rng) < params.crossover_rate) {
            int crossover_point = point_dist(rng);
            for (int j = crossover_point; j < 4; j++) {
                std::swap(population[i].genes[j], population[i+1].genes[j]);
            }
        }
    }
}
```

**6. Mutatie**
* Pentru fiecare gena a copilului, exita o sansa egala cu *mutation_rate* (10%) ca aceasta sa fie modificata cu o valoare random (conforma cu intervalul initial)
```cpp
void GeneticValuePredictor::mutation() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> history_dist(1, 10);
    std::uniform_int_distribution<int> confidence_dist(1, 5);
    std::uniform_int_distribution<int> stride_dist(1, 5);
    std::uniform_int_distribution<int> reuse_dist(0, 99);

    for (auto& chromosome : population) {
        if (dist(rng) < params.mutation_rate) {
            int gene = rand() % 4;
            switch (gene) {
                case 0: chromosome.genes[0] = history_dist(rng); break;
                case 1: chromosome.genes[1] = confidence_dist(rng); break;
                case 2: chromosome.genes[2] = stride_dist(rng); break;
                case 3: chromosome.genes[3] = reuse_dist(rng); break;
            }
        }
    }
}
```
* Aceasta aduce variatie in cadrul populatiei

**7. Tournament Selection**
* Acesta selecteaza aleator 5 indivizi (tournament_size)
* Returneaza individul cu cel mai bun fitness
```cpp
Chromosome GeneticValuePredictor::tournamentSelection() {
    std::uniform_int_distribution<int> dist(0, population.size() - 1);
    Chromosome best;
    best.fitness = -1;

    for (int i = 0; i < params.tournament_size; i++) {
        int idx = dist(rng);
        if (population[idx].fitness > best.fitness) {
            best = population[idx];
        }
    }

    return best;
}
```

**8. Decode chromosome**
* Extrage cele 4 gene din individ
```cpp
PredictorParams GeneticValuePredictor::decodeChromosome(const Chromosome& chromosome) {
    PredictorParams params;
    params.history_size = chromosome.genes[0];
    params.confidence_threshold = chromosome.genes[1];
    params.stride_window = chromosome.genes[2];
    params.reuse_threshold = chromosome.genes[3] / 100.0;
    return params;
}
```
**9. Predictie**
* Cauta istoricul valorilor pentru uop
* Daca sunt destule valori:
  * verifica daca exitsta un stride constant intre ultimele valori
  * da -> prezice prin extrapolare cu stride
  * nu -> copiaza ultima valoare
* Returneaza true daca a reusit sa faca o predictie si false daca nu
```cpp
bool GeneticValuePredictor::predictWithParams(const PredictorParams& params, Uop* uop, long long& predicted_value) {
    long long uop_id = uop->getId();
    auto it = prediction_table.find(uop_id);
    
    if (it == prediction_table.end())
        return false;
        
    ValueHistory& history = it->second;
    
    if (history.values.size() < params.history_size)
        return false;
        
    bool has_constant_stride = true;
    int stride = history.values[1] - history.values[0];
    
    for (size_t i = 2; i < std::min(history.values.size(), static_cast<size_t>(params.stride_window)); i++) {
        if (history.values[i] - history.values[i-1] != stride) {
            has_constant_stride = false;
            break;
        }
    }
    
    if (has_constant_stride) {
        predicted_value = history.values.back() + stride;
        history.stride = stride;
    } else {
        predicted_value = history.values.back();
    }

    history.last_prediction = predicted_value;
    history.has_prediction = true;
    
    return true;
}
```

**10. Update**
* Actualizeaza istoricul valorilor pentru un uop dupa ce s-a aflat valoarea reala
* Daca exista o predictie anterioara:
  * numarul total de predictii creste
  * daca : predictie corecta : correct_predictions ++
  * daca : predictie corecta si confidenta : correct_confident_predictions ++
* Adauga valoarea reala la istoricul uop
* Mentine istoricul la max history_size valori
```cpp
void GeneticValuePredictor::update(Uop* uop, long long actual_value) {
    long long uop_id = uop->getId();
    ValueHistory& history = prediction_table[uop_id];
    if (history.has_prediction) {
        total_predictions++;  
        if (history.last_prediction == actual_value) {
            correct_predictions++;
            if (isConfidentPrediction(uop)) {
                correct_confident_predictions++;
            }
        }
        history.has_prediction = false;
    }
    history.values.push_back(actual_value);
    if (history.values.size() > current_params.history_size)
        history.values.erase(history.values.begin());
    if (history.values.size() >= 2) {
        int current_stride = history.values.back() - history.values[history.values.size()-2];
        if (current_stride == history.stride)
            history.confidence++;
        else
            history.confidence = 0;
    }
}
```
**11.Train**
* Antreneaza predictorul folosind algoritmul genetic
* Pentru fiecare generatie:
  * Calculeaza fitness-ul pentru fiecare individ
  * Selecteaza indivizii parinti
  * Crossover
  * Mutatie
* Selecteaza cel mai bun individ si ii aplica genele in current_params
```cpp
void GeneticValuePredictor::train() {
    initializePopulation();
    
    for (int gen = 0; gen < params.generations; gen++) {
        for (auto& chromosome : population) {
            chromosome.fitness = evaluateFitness(chromosome);
        }
        
        selection();
        
        crossover();
        
        mutation();
    }
    
    auto best = std::max_element(population.begin(), population.end(),
        [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });
    
    current_params = decodeChromosome(*best);
}
```
## Rezultate
### 1. Procentajul de instructiuni triviale din suita de benchmark-uri Parsec
![Trivial](graph_images/Trivial.png "Trivial")
### 2. Procentajul de instructiuni ALU reutilizate din suita de benchmark-uri Parsec
![Trivial](graph_images/alu.png "Trivial")
### 3. Procentajul de instructiuni Load reutilizate din suita de benchmark-uri Parsec
![Trivial](graph_images/load.png "Trivial")
### 4. Overall accuracy of Value Predictor vs Genetic Value Predictor
![Trivial](graph_images/pvp.png "")
### Bibliografie 
1. Multi2Sim Github Repository:https://github.com/Multi2Sim/multi2sim 
2. Parsec Github Repository:https://github.com/Multi2Sim/m2s-bench-parsec-3.0-src 
3. Multi2Sim Official Documentation:http://www.multi2sim.org/downloads/m2s-guide-4.2.pdf 
