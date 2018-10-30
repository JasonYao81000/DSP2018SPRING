#include "hmm.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  // Load file name of modellist.txt from arguments[1].
  char* model_list_name = argv[1];
  // Load file name of testing_data*.txt from arguments[2].
  char* testing_data_name = argv[2];
  // Load file name of result.txt from arguments[3].
  char* result_name = argv[3];

  // We allow max. of 10 models.
  int max_num_models = 10;
  // Create HMM models.
  HMM models[max_num_models];
  // Load models and get number of models.
  int num_models = load_models(model_list_name, models, max_num_models);

  // Try to open file (testing_data).
  FILE* testing_data_file = fopen(testing_data_name, "r");
  // If file pointer if null, print error message and exit.
  if (testing_data_file == NULL) {
    perror("Failed to open file (testing_data*.txt)");
    exit(1);
  }

  // Try to open file (result.txt).
  FILE* result_file = fopen(result_name, "w");
  // If file pointer if null, print error message and exit.
  if (result_file == NULL) {
    perror("Failed to open file (result.txt)");
    exit(1);
  }

  // Buffer of each sample with MAX_LINE size.
  char sample[MAX_LINE];
  // Reset file seek to row 0 and colume 0 .
  fseek(testing_data_file, 0, SEEK_SET);
  // Keep reading samples line by line.
  while (fscanf(testing_data_file, "%s", sample) > 0) {
    // Get length of sample (observation).
    int length_sample = strlen(sample);

    // Observations with the same length of sample.
    int observations[length_sample];
    // Convert sample (A-F) to observation (0~5).
    for (int t = 0; t < length_sample; t++) {
      observations[t] = sample[t] - 'A';
    }

    // Record which model has max. likelihood.
    int max_likelihood_model_index = -1;
    // Record max. likelihood.
    double max_likelihood = 0;
    // Delta of viterbi algorithm.
    double delta[length_sample][MAX_STATE];

    // Test each model.
    for (int model_index = 0; model_index < num_models; model_index++) {
      // Get HMM model from models array.
      HMM model = models[model_index];
      // Get number of state from HMM model.
      int num_states = model.state_num;

      // Initialization of delta (0, i).
      for (int i = 0; i < num_states; i++) {
        delta[0][i] = model.initial[i] * model.observation[observations[0]][i];
      }

      // Induction of delta (1 ~ *, i).
      for (int t = 1; t < length_sample; t++) {
        for (int j = 0; j < num_states; j++) {
          // Try to find max. path probability from state[i] to state[j].
          double max_path_prob = 0.0;
          for (int i = 0; i < num_states; i++) {
            // Calculate path probability of next time step.
            double path_prob = delta[t - 1][i] * model.transition[i][j];
            // Update max. path probability.
            if (max_path_prob < path_prob) max_path_prob = path_prob;
          }
          // Update delta with max. path probability.
          delta[t][j] = max_path_prob * model.observation[observations[t]][j];
        }
      }

      // Try to find max. path probability of time T (last).
      double max_path_prob = 0.0;
      for (int i = 0; i < num_states; i++) {
        // Update max. path probability.
        if (max_path_prob < delta[length_sample - 1][i]) max_path_prob = delta[length_sample - 1][i];
      }

      // Update max. likelihood and model index.
      if (max_likelihood < max_path_prob) {
        max_likelihood = max_path_prob;
        max_likelihood_model_index = model_index;
      }
    }

    // Write model name and its likelihood to file.
    fprintf(result_file, "%s %e\n", models[max_likelihood_model_index].model_name, max_likelihood);
  }

  // Close opened files.
  fclose(testing_data_file);
  fclose(result_file);

  return 0;
}