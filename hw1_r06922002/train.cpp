#include "hmm.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  // Load iterations from arguments[1].
  int iterations = strtol(argv[1], NULL, 10);
  // Load file name of model_init.txt from arguments[2].
  char* model_init = argv[2];
  // Load file name of seq_model_01~05.txt from arguments[3].
  char* seq_model = argv[3];
  // Load file name of model_01~05.txt from arguments[4].
  char* model_name = argv[4];

  // Create a HMM model.
  HMM model;
  // Initialize the HMM model.
  loadHMM(&model, model_init);

  // Try to open file (input sequence of model).
  FILE* input_seq_file = fopen(seq_model, "r");
  // If file pointer if null, print error message and exit.
  if (input_seq_file == NULL) {
    perror("Failed to open file (seq_model_01~05.txt)");
    exit(1);
  }

  // Train model 'iterations' times.
  for (int i = 0; i < iterations; ++i) {
    // Alpha, beta, gamma and epsilon of each sample, pi will accumulate through all samples.
    double alpha[MAX_SEQ][MAX_STATE];
    double beta[MAX_SEQ][MAX_STATE];
    double gamma[MAX_SEQ][MAX_STATE];
    double epsilon[MAX_SEQ - 1][MAX_STATE][MAX_STATE];
    double accumulate_pi[MAX_STATE];
    double sum_gamma[MAX_STATE][MAX_OBSERV];
    double sum_gamma_T[MAX_STATE];
    double sum_epsilon[MAX_STATE][MAX_STATE];

    // Get number of state and observation from HMM model.
    int num_states = model.state_num;
    int num_observations = model.observ_num;

    // Clear all accumulated values.
    for (int i = 0; i < num_states; i++) {
      accumulate_pi[i] = 0;
      sum_gamma_T[i] = 0;
      for (int j = 0; j < num_observations; j++) {
        sum_gamma[i][j] = 0;
      }
      for (int j = 0; j < num_states; j++) {
        sum_epsilon[i][j] = 0;
      }
    }

    // Clear samples counter.
    int num_samples = 0;
    // Buffer of each sample with MAX_LINE size.
    char sample[MAX_LINE];
    // Reset file seek to row 0 and colume 0 .
    fseek(input_seq_file, 0, SEEK_SET);
    // Keep reading samples line by line.
    while (fscanf(input_seq_file, "%s", sample) > 0) {
      // Get length of sample (observation).
      int length_sample = strlen(sample);

      // Observations with the same length of sample.
      int observations[length_sample];
      // Convert sample (A-F) to observation (0~5).
      for (int t = 0; t < length_sample; t++) {
        observations[t] = sample[t] - 'A';
      }

      // Clear alpha, beta, gamma and epsilon of each sample.
      for (int t = 0; t < length_sample; t++) {
        for (int i = 0; i < num_states; i++) {
          alpha[t][i] = 0;
          beta[t][i] = 0;
          gamma[t][i] = 0;
          for (int j = 0; j < num_states; j++) {
            epsilon[t][i][j] = 0;
          }
        }
      }

      // Calculate alpha using forward procedure.
      // Initialization of alpha (0, i).
      for (int i = 0; i < num_states; i++) {
        alpha[0][i] = model.initial[i] * model.observation[observations[0]][i];
      }
      // Induction of alpha (1 ~ *, i).
      for (int t = 1; t < length_sample; t++) {
        for (int j = 0; j < num_states; j++) {
          double sum = 0;
          for (int i = 0; i < num_states; i++) {
            sum += alpha[t - 1][i] * model.transition[i][j];
          }
          alpha[t][j] = sum * model.observation[observations[t]][j];
        }
      }

      // Calculate beta using backward procedure.
      // Initialization of beta (T - 1, i).
      for (int i = 0; i < num_states; i++) {
        beta[length_sample - 1][i] = 1;
      }
      // Induction of alpha (T - 2 ~ 0, i).
      for (int t = length_sample - 2; t >= 0; t--) {
        for (int i = 0; i < num_states; i++) {
          double sum = 0;
          for (int j = 0; j < num_states; j++) {
            sum += model.transition[i][j] * model.observation[observations[t + 1]][j] * beta[t + 1][j];
          }
          beta[t][i] = sum;
        }
      }

      // Calculate gamma using element-wise multiply and normalize.
      for (int t = 0; t < length_sample; t++) {
        double sum = 0;
        // Element-wise multiply and sum of gamma over t.
        for (int i = 0; i < num_states; i++) {
          gamma[t][i] = alpha[t][i] * beta[t][i];
          sum += gamma[t][i];
        }
        // Normalize on gamma.
        for (int i = 0; i < num_states; i++) {
          gamma[t][i] = gamma[t][i] / sum;
        }
      }

      // Calculate epsilon using element-wise multiply and normalize.
      for (int t = 0; t < length_sample - 1; t++) {
        double sum = 0;
        // Element-wise multiply and sum of epsilon over t.
        for (int i = 0; i < num_states; i++) {
          for (int j = 0; j < num_states; j++) {
            epsilon[t][i][j] = alpha[t][i] * model.transition[i][j] * model.observation[observations[t + 1]][j] * beta[t + 1][j];
            sum += epsilon[t][i][j];
          }
        }
        // Normalize on epsilon.
        for (int i = 0; i < num_states; i++) {
          for (int j = 0; j < num_states; j++) {
            epsilon[t][i][j] = epsilon[t][i][j] / sum;
          }
        }
      }

      // Accumulate pi of each sample.
      for (int i = 0; i < num_states; i++) {
        accumulate_pi[i] += gamma[0][i];
      }

      // Accumulate gamma of each sample.
      for (int i = 0; i < num_states; i++) {
        for (int t = 0; t < length_sample; t++) {
          sum_gamma[i][observations[t]] += gamma[t][i];
        }
        sum_gamma_T[i] += gamma[length_sample - 1][i];
      }

      // Accumulate epsilon of each sample.
      for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_states; j++) {
          double sum = 0;
          for (int t = 0; t < length_sample - 1; t++) {
            sum += epsilon[t][i][j];
          }
          sum_epsilon[i][j] += sum;
        }
      }
      // Count number of samples.
      num_samples++;
    }

    // Accumulate epsilon and gamma through all samples, then update model.
    // Update initial probability of model (pi).
    for (int i = 0; i < num_states; i++) {
      model.initial[i] = accumulate_pi[i] / num_samples;
    }

    // Updata transition probability of model (a[i][j]).
    for (int i = 0; i < num_states; i++) {
      double sum = 0;
      for (int j = 0; j < num_observations; j++) {
        sum += sum_gamma[i][j];
      }
      sum -= sum_gamma_T[i];
      for (int j = 0; j < num_states; j++) {
        model.transition[i][j] = sum_epsilon[i][j] / sum;
      }
    }

    // Updata emission probability of model (b[i][k]).
    for (int i = 0; i < num_states; i++) {
      double sum = 0;
      for (int j = 0; j < num_observations; j++) {
        sum += sum_gamma[i][j];
      }
      for (int j = 0; j < num_observations; j++) {
        model.observation[j][i] = sum_gamma[i][j] / sum;
      }
    }
  }

  // Close opened file.
  fclose(input_seq_file);

  // Try to open file (output model).
  FILE* output_model_file = fopen(model_name, "w");
  // If file pointer if null, print error message and exit.
  if (output_model_file == NULL) {
    perror("Failed to open file (output model)");
    exit(1);
  }

  // Dump full HMM model to txt file.
  dumpHMM(output_model_file, &model);

  // Close opened file.
  fclose(output_model_file);

  return 0;
}