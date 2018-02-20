% StartUp 
% Sample
% Check 

ARCHI_TYPE wcs(
	
	const rate SampleWaterCompanyServer_rate := 0.000355,
	const rate StartUpWaterCompanyServer_rate := 0.84,
	const rate CheckWaterCompanyServer_rate := 0.625,
	const rate SampleWaterCompanyDisk_rate := 0.025,
	const rate StartUpWaterCompanyDisk_rate := 25,
	const rate CheckWaterCompanyDisk_rate := 12.5,
	const rate WiredConnectionUp_rate := 0.02,
	const rate WiredConnectionDown_rate := 0.02,
	const rate QSSamplingApp_request_rate := 0,
	const rate QSQualityControlApp_request_rate := 0,
	const rate CCS_send_StartUpSampling_request_rate,  
	const rate CCS_CheckWaterInland_prob_request, 
	const rate CCS_send_wired_getExpectedParametersInland_request_rate,
	const rate CCS_send_wired_getExpectedParametersOutGoing_request_rate,
	const rate CCS_send_wired_StartUpSampling_response_rate, 
	const rate CCS_send_sample_rate, 
	const rate CCS_send_wired_CheckWaterInland_response_rate,
	const rate CCS_send_wired_CheckWaterOutGoing_response_rate

)

ARCHI_ELEM_TYPES

	% Water Company Server

	ELEM_TYPE WaterCompanyServer_Type(const rate SampleWaterCompanyServer_rate, const rate CheckWaterCompanyServer_rate)

		BEHAVIOR

			ServerCalled (void;void) =
				choice{
					<save_sample_request, _> . <save_sample_Db, exp(SampleWaterCompanyServer_rate)> . ServerCalled(),
					<getExpectedParametersOutGoing_request, _> . <getExpectedParametersOutGoing_db_request, exp(CheckWaterCompanyServer_rate)> . WaitingParamsRequest()
				};


			WaitingParamsRequest (void;void) = 
				<getExpectedParametersOutGoing_db_response, _> . <getExpectedParametersOutGoing_response, inf> . ServerCalled()
				

		INPUT_INTERACTIONS

			UNI save_sample_request;
				getExpectedParametersOutGoing_request;
				getExpectedParametersOutGoing_db_response

		OUTPUT_INTERACTIONS

			UNI getExpectedParametersOutGoing_response;
				save_sample_Db;
				getExpectedParametersOutGoing_db_request



	% Wired Connection Up

	ELEM_TYPE WiredConnectionUp_Type(const rate WiredConnectionUp_rate)

		BEHAVIOR

			WiredCalled(void;void) =
				choice{
					<receive_qss_response, _> . <forward_qss, exp(WiredConnectionUp_rate)> . WiredCalled(),
					<receive_params_response, _> . <forward_params_response, exp(WiredConnectionUp_rate)> . WiredCalled(),
					<receive_qsqc_response, _> . <forward_qsqc, exp(WiredConnectionUp_rate)> . WiredCalled()
				}


		INPUT_INTERACTIONS

			UNI receive_qss_response;
				receive_params_response;
				receive_qsqc_response

		OUTPUT_INTERACTIONS

			UNI forward_qss;
				forward_params_response;
				forward_qsqc


		% Wired Connection Down

		ELEM_TYPE WiredConnectionDown_Type(const rate WiredConnectionDown_rate)

		BEHAVIOR

			WiredCalled(void;void) =
				choice{
					<receive_qss_request, _> . <forward_qss, exp(WiredConnectionDown_rate)> . WiredCalled(),
					<receive_params_request, _> . <forward_params_request, exp(WiredConnectionDown_rate)> . WiredCalled(),
					<receive_qsqc_request, _> . <forward_qsqc, exp(WiredConnectionDown_rate)> . WiredCalled(),
					<receive_save_sample_request, _> . <forward_save_sample_request, exp(WiredConnectionDown_rate)> . WiredCalled()
				}


		INPUT_INTERACTIONS

			UNI receive_qss_request;
				receive_params_request;
				receive_qsqc_request;
				receive_save_sample_request

		OUTPUT_INTERACTIONS

			UNI forward_qss;
				forward_params_request;
				forward_qsqc;
				forward_save_sample_request


		% Water Company Disk

		ELEM_TYPE WaterCompanyDisk_Type(const rate SampleWaterCompanyDisk_rate, const rate CheckWaterCompanyDisk_rate)

		BEHAVIOR

			DiskCalled(void;void) =
				choice{
					<save_sample_Db, _> . DiskCalled(), %invece dei underscore ci va il tempo del disco
					<getExpectedParametersOutGoing_db_request, _> . <getExpectedParametersOutGoing_db_response, exp(CheckWaterCompanyDisk_rate)> . DiskCalled()
				}


		INPUT_INTERACTIONS 

			UNI save_sample_Db;
				getExpectedParametersOutGoing_db_request

		OUTPUT_INTERACTIONS

			UNI getExpectedParametersOutGoing_db_response



		% QSSamplingApp

		ELEM_TYPE QSSamplingApp_Type(const rate QSSamplingApp_request_rate)

		BEHAVIOR

			AppCalled(void;void) =
				<generate_sampling_request, exp(QSSamplingApp_request_rate)> . <send_request, inf> . WaitForResponse();

			WaitForResponse(void;void) =
				<receive_response, _> .AppCalled()


		INPUT_INTERACTIONS

			UNI generate_sampling_request;
				receive_response

		OUTPUT_INTERACTIONS

			UNI send_request



		%QSQualityControlApp

		ELEM_TYPE QSQualityControlApp_Type(const rate QSQualityControlApp_request_rate)

		BEHAVIOR

			AppCalled(void;void) =
				<generate_checkWaterQuality_request, exp(QSQualityControlApp_request_rate)> . <send_request, inf> . WaitForResponse();

			WaitForResponse(void;void) =
				<receive_response, _> .AppCalled()

		INPUT_INTERACTIONS

			UNI generate_checkWaterQuality_request;
				receive_response

		OUTPUT_INTERACTIONS

			UNI send_request



		%ControlCenterServer

		ELEM_TYPE ControlCenterServer_Type(const rate CCS_send_StartUpSampling_request_rate,  const rate CCS_CheckWaterInland_prob_request, 
			const rate CCS_send_wired_getExpectedParametersInland_request_rate, const rate CCS_send_wired_getExpectedParametersOutGoing_request_rate,
			const rate CCS_send_wired_StartUpSampling_response_rate, const rate CCS_send_sample_rate, const rate CCS_send_wired_CheckWaterInland_response_rate,
			const rate CCS_send_wired_CheckWaterOutGoing_response_rate)

		BEHAVIOR

			ServerCalled(void;void) = 
				choice{
					<receive_wired_StartUpSampling_request, _> . <send_StartUpSampling_request, exp(CCS_send_StartUpSampling_request_rate)> .WaitingStartUpSampling(),
					<receive_wired_CheckWater_request, _> . 
						choice{
							<inland, inf(1, CCS_CheckWaterInland_prob_request)> . <send_wired_getExpectedParametersInland_request, exp(CCS_send_wired_getExpectedParametersInland_request_rate)> . WaitingForInParameters(),
							<outGoing, inf(1, 1- CCS_CheckWaterInland_prob_request)> . <send_wired_getExpectedParametersOutgoing_request, exp(CCS_send_wired_getExpectedParametersOutGoing_request_rate)> . WaitingForOUTParameters()

						}
			}

			WaitingStartUpSampling(void;void) =
				<receive_StartUpSampling_response, _> . <send_wired_StartUpSampling_response, exp(CCS_send_wired_StartUpSampling_response_rate)> .WaitingForSample();

			WaitingForSample(void;void) =
				<receive_sample, _> . <send_sample, exp(CCS_send_sample_rate)> . ServerCalled();

			WaitingForInParameters(void;void) = 
				<receive_wired_getExpectedParametersInland_response, _> . <send_wired_CheckWater_response, exp(CCS_send_wired_CheckWater_responseInland_rate)> .ServerCalled();


			WaitingForOutParameters(void;void) = 
				<receive_wired_getExpectedParametersOutGoing_response, _> . <send_wired_CheckWater_response, exp(CCS_send_wired_CheckWater_responseOutGoing_rate)> .ServerCalled()




		INPUT_INTERACTIONS

			UNI receive_wired_StartUpSampling_request,
				receive_wired_CheckWater_request,
				receive_wired_getExpectedParametersInland_response,
				receive_wired_getExpectedParametersOutGoing_response,

			OR  receive_StartUpSampling_response,
				receive_sample

		OUTPUT_INTERACTIONS

			UNI send_wired_StartUpSampling_response,
				send_wired_CheckWater_response,
				send_wired_getExpectedParametersInland_request,
				send_wired_getExpectedParametersOutGoing_request,
				send_sample,

			OR 	send_StartUpSampling_request



ARCHI_TOPOLOGY

	ARCHI_ELEM_INSTANCES
		WCS : WaterCompanyServer_Type(SampleWaterCompanyServer_rate, CheckWaterCompanyServer_rate);
		WCWCU : WiredConnectionUp_Type(WiredConnectionUp_rate);
		WCWCD : WiredConnectionDown_Type(WiredConnectionDown_rate);
		WCD : WaterCompanyDisk_Type(SampleWaterCompanyDisk_rate, CheckWaterCompanyDisk_rate)



	ARCHI_INTERACTIONS

		WCS.save_sample_request;
		WCS.getExpectedParametersOutGoing_request


	ARCHI_ATTACHMENTS

	FROM WCS.save_sample_Db 							TO WCWCD.receive_save_sample_request;
	FROM WCS.getExpectedParametersOutGoing_db_request 	TO WCWCD.receive_params_request;
	FROM WCWCU.forward_params_response					TO WCS.getExpectedParametersOutGoing_db_response;
	FROM WCWCD.forward_save_sample_request				TO WCD.save_sample_Db;
	FROM WCWCD.forward_params_request					TO WCD.getExpectedParametersOutGoing_db_request;
	FROM WCD.getExpectedParametersOutGoing_db_response  TO WCWCU.receive_params_response

END