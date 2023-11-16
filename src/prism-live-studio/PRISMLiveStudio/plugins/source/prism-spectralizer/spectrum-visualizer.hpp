/*************************************************************************
 * This file is part of spectralizer
 * github.con/univrsal/spectralizer
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once
#include "util.hpp"
#include "audio-visualizer.hpp"
#include "prism-visualizer-source.hpp"
#include <vector>

const int DEAD_BAR_OFFSET = 0; /* The last five bars seem to always be silent, so we cut them off */

/* Save some writing */
using doublev = std::vector<double>;
using uint32v = std::vector<uint32_t>;

class spectrum_visualizer : public audio_visualizer {
private:
	config *m_cfg = nullptr;
	uint32_t m_last_bar_count = 0;
	bool m_sleeping = false;
	float m_sleep_count = 0.f;
	/* fft calculation vars */
	size_t m_fftw_results = 0;
	double *m_fftw_input_left = nullptr;
	double *m_fftw_input_right = nullptr;

	fftw_complex *m_fftw_output_left = nullptr;
	fftw_complex *m_fftw_output_right = nullptr;

	fftw_plan m_fftw_plan_left = nullptr;
	fftw_plan m_fftw_plan_right = nullptr;

	/* Frequency cutoff variables */
	uint32v m_low_cutoff_frequencies;
	uint32v m_high_cutoff_frequencies;
	doublev m_frequency_constants_per_bin;

	uint64_t m_silent_runs = 0u; /* determines sleep state */

	uint32_t m_last_sample_size = 0;

	bool prepare_fft_input(const pcm_stereo_sample *buffer, uint32_t sample_size, double *fftw_input, channel_mode channel_mode) const;

	void create_spectrum_bars(fftw_complex *fftw_output, size_t fftw_results, int32_t win_height, uint32_t number_of_bars, doublev *bars, const doublev *bars_falloff);

	void generate_bars(uint32_t number_of_bars, size_t fftw_results, const uint32v &low_cutoff_frequencies, const uint32v &high_cutoff_frequencies, const fftw_complex *fftw_output,
			   doublev *bars) const;

	void recalculate_cutoff_frequencies(uint32_t number_of_bars, uint32v *low_cutoff_frequencies, uint32v *high_cutoff_frequencies, doublev *freqconst_per_bin) const;
	void smooth_bars(doublev *bars);
	void apply_falloff(const doublev &bars, doublev *falloff_bars) const;
	void calculate_moving_average_and_std_dev(double new_value, size_t max_number_of_elements, doublev *old_values, double *moving_average, double *std_dev) const;
	void maybe_reset_scaling_window(double current_max_height, size_t max_number_of_elements, doublev *values, double *moving_average, double *std_dev) const;
	void scale_bars(int32_t height, doublev *bars);
	void sgs_smoothing(doublev *bars);
	void monstercat_smoothing(doublev *bars);
	void monstercat_calculate_weight(doublev *bars, int index_);
	bool assign_bars_data(float seconds);

	/* New values are smoothly copied over if smoothing is used
     * otherwise they're directly copied */
	doublev m_bars_left;
	doublev m_bars_right;
	doublev m_bars_left_new;
	doublev m_bars_right_new;
	doublev m_bars_falloff_left;
	doublev m_bars_falloff_right;
	doublev m_previous_max_heights;
	doublev m_monstercat_smoothing_weights;

	float m_corner_radius = 0;
	std::vector<struct vec2> m_circle_points;

public:
	explicit spectrum_visualizer(config *cfg);

	~spectrum_visualizer() override;

	spectrum_visualizer(spectrum_visualizer const &) = delete;
	spectrum_visualizer &operator=(spectrum_visualizer const &) = delete;

	spectrum_visualizer(spectrum_visualizer &&) = delete;
	spectrum_visualizer &operator=(spectrum_visualizer &&) = delete;

	void update() final;

	void tick(float seconds) override;

	doublev &get_bars_left() { return m_bars_left; }
	doublev &get_bars_right() { return m_bars_right; }

	gs_vertbuffer_t *make_rounded_rectangle(float height, bool circle = false);
};
