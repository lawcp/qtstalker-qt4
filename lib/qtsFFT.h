#ifndef qtsFFT_H
#define qtsFFT_H

#include "PlotLine.h"


class FFTReal;

class qtsFFT
{

public:

    qtsFFT(long n);
	~qtsFFT();

    PlotLine * do_FFTqts(PlotLine *r);
    PlotLine * do_iFFTqts(PlotLine *f);

private:

    int length;

    double * fftReal;       // qtsFFT arrays
    double * fftFreq;

//
// FFTReal by Laurent de Soras
// Modified to nested class for inclusion in this module
// 	-- jim nolen jnolen1@mindspring.com, 29 Nov 2004
//

// --- nested Class FFTReal ----
    class FFTReal
    {
		public:

			/* Change this typedef to use a different floating point type in your FFTs
			(i.e. float, double or long double). */
			typedef double	flt_t;

			explicit	FFTReal (const long length);
						~FFTReal ();
			void		do_fft (flt_t f [], const flt_t x []) const;
			void		do_ifft (const flt_t f [], flt_t x []) const;
			void		rescale (flt_t x []) const;

		private:

			/* Bit-reversed look-up table nested class */
			class BitReversedLUT
			{
				public:

					explicit		BitReversedLUT (const int nbr_bits);
									~BitReversedLUT ();
					const long *	get_ptr () const
					{
					return (_ptr);
					}
				private:
					long *			_ptr;
			};

			/* Trigonometric look-up table nested class */
			class	TrigoLUT
			{
				public:

					explicit		TrigoLUT (const int nbr_bits);
									~TrigoLUT ();
					const flt_t	*	get_ptr (const int level) const
					{
						return (_ptr + (1L << (level - 1)) - 4);
					};
			private:
				flt_t	*			_ptr;
			};

		const long		_length;
        	const int		_nbr_bits;
		const BitReversedLUT	_bit_rev_lut;
		const TrigoLUT	_trigo_lut;
		const flt_t		_sqrt2_2;
		flt_t *			_buffer_ptr;

		};

	const FFTReal _ftt_Real;

};

#endif
