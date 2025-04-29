#!/bin/bash

# Output CSV file
output_file="results.csv"

# Write CSV header
echo "Benchmark,ALU Reuse %,Load Reuse %,Trivial %,Genetic Predictor Accuracy %,Value Predictor Accuracy %" >"$output_file"

# Loop over all output_*.txt files
for file in output_*.txt; do
  # Extract the benchmark name (remove output_ and .txt)
  benchmark=$(basename "$file" .txt)
  benchmark=${benchmark#output_}

  # Extract required fields
  alu_reuse=$(grep "ALU Reuse Percentage" "$file" | awk '{print $5}' | tr -d '%')
  load_reuse=$(grep "Load Reuse Percentage" "$file" | awk '{print $5}' | tr -d '%')
  trivial_percentage=$(grep "Trivial Percentage" "$file" | awk '{print $4}' | tr -d '%')
  genetic_accuracy=$(grep -A 6 "Genetic Value Predictor Statistics" "$file" | grep "Overall Accuracy" | awk '{print $3}' | tr -d '%')
  value_accuracy=$(grep -A 7 "Value Predictor Statistics" "$file" | grep "Overall Accuracy" | awk '{print $3}' | tr -d '%')

  # In case some values are missing, set them to 0
  alu_reuse=${alu_reuse:-0}
  load_reuse=${load_reuse:-0}
  trivial_percentage=${trivial_percentage:-0}
  genetic_accuracy=${genetic_accuracy:-0}
  value_accuracy=${value_accuracy:-0}

  # Write to CSV
  echo "$benchmark,$alu_reuse,$load_reuse,$trivial_percentage,$genetic_accuracy,$value_accuracy" >>"$output_file"
done

echo "Extraction complete. Results saved to $output_file."
