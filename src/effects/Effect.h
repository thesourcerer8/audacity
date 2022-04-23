/**********************************************************************

  Audacity: A Digital Audio Editor

  Effect.h

  Dominic Mazzoni
  Vaughan Johnson

**********************************************************************/

#ifndef __AUDACITY_EFFECT__
#define __AUDACITY_EFFECT__

#include "EffectBase.h"

#include "SampleCount.h"

#define BUILTIN_EFFECT_PREFIX wxT("Built-in Effect: ")

class EffectParameterMethods;
class LabelTrack;
class WaveTrack;

//! A mix-in class for effects that are not yet migrated to statelessness.
//! To be eliminated when all effects are migrated
class AUDACITY_DLL_API StatefulEffectBase {
public:
   //! Calls through to members of StatefulEffectBase
   class AUDACITY_DLL_API Instance : public virtual EffectInstance {
   public:
      explicit Instance(StatefulEffectBase &effect);
      ~Instance() override;

      bool Init() override;

      void SetSampleRate(double rate) override;
   
      size_t GetBlockSize() const override;
      size_t SetBlockSize(size_t maxBlockSize) override;
   
      bool RealtimeInitialize(EffectSettings &settings) override;
      bool RealtimeAddProcessor(EffectSettings &settings,
         unsigned numChannels, float sampleRate) override;
      bool RealtimeSuspend() override;
      bool RealtimeResume() noexcept override;
      bool RealtimeProcessStart(EffectSettings &settings) override;
      size_t RealtimeProcess(int group, EffectSettings &settings,
         const float *const *inBuf, float *const *outBuf, size_t numSamples)
      override;
      bool RealtimeProcessEnd(EffectSettings &settings) noexcept override;
      bool RealtimeFinalize(EffectSettings &settings) noexcept override;
   protected:
      StatefulEffectBase &mEffect;
      StatefulEffectBase &GetEffect() const { return mEffect; }
   };

   /*!
     @copydoc EffectInstance::Init()
     Default implementation does nothing, returns true
   */
   virtual bool Init();

   /*!
    @copydoc EffectInstance::Process
    */
   virtual bool Process(EffectInstance &instance, EffectSettings &settings) = 0;

   /*!
     @copydoc EffectInstance::SetSampleRate()
     Default implementation assigns mSampleRate
   */
   virtual void SetSampleRate(double rate);

   /*!
     @copydoc RealtimeInitialize::RealtimeInitialize()
     Default implementation does nothing, returns false
   */
   virtual bool RealtimeInitialize(EffectSettings &settings);

   /*!
     @copydoc RealtimeInitialize::RealtimeAddProcessor()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeAddProcessor(
      EffectSettings &settings, unsigned numChannels, float sampleRate);

   /*!
     @copydoc RealtimeInitialize::RealtimeSuspend()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeSuspend();

   /*!
     @copydoc RealtimeInitialize::RealtimeResume()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeResume() noexcept;

   /*!
     @copydoc RealtimeInitialize::RealtimeProcessStart()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeProcessStart(EffectSettings &settings);

   /*!
     @copydoc RealtimeInitialize::RealtimeProcess()
     Default implementation does nothing, returns 0
   */
   virtual size_t RealtimeProcess(int group, EffectSettings &settings,
      const float *const *inBuf, float *const *outBuf, size_t numSamples);

   /*!
     @copydoc RealtimeInitialize::RealtimeProcessEnd()
     Default implementation does nothing, returns true
   */
   virtual bool RealtimeProcessEnd(EffectSettings &settings) noexcept;

   /*!
     @copydoc RealtimeInitialize::RealtimeFinalize()
     Default implementation does nothing, returns false
   */
   virtual bool RealtimeFinalize(EffectSettings &settings) noexcept;

   /*!
     @copydoc RealtimeInitialize::SetBlockSize()
     Default implementation assigns mEffectBlockSize, returns it
   */
   virtual size_t SetBlockSize(size_t maxBlockSize);

   /*!
     @copydoc RealtimeInitialize::GetBlockSize()
     Default implementation returns mEffectBlockSize
   */
   virtual size_t GetBlockSize() const;

protected:

   double         mSampleRate{};
private:

   size_t mEffectBlockSize{ 0 };
};

class AUDACITY_DLL_API Effect /* not final */
   : public wxEvtHandler
   , public EffectBase
{
 //
 // public methods
 //
 // Used by the outside program to determine properties of an effect and
 // apply the effect to one or more tracks.
 //
 public:
   static inline Effect *FetchParameters(Effect &e, EffectSettings &)
   { return &e; }

   // The constructor is called once by each subclass at the beginning of the program.
   // Avoid allocating memory or doing time-consuming processing here.
   Effect();
   virtual ~Effect();

   // ComponentInterface implementation

   PluginPath GetPath() const override;
   bool VisitSettings(
      SettingsVisitor &visitor, EffectSettings &settings) override;
   bool VisitSettings(
      ConstSettingsVisitor &visitor, const EffectSettings &settings)
      const override;

   ComponentInterfaceSymbol GetSymbol() const override;

   VendorSymbol GetVendor() const override;
   wxString GetVersion() const override;
   TranslatableString GetDescription() const override;

   // EffectDefinitionInterface implementation

   EffectType GetType() const override;
   EffectFamilySymbol GetFamily() const override;
   bool IsInteractive() const override;
   bool IsDefault() const override;
   bool SupportsRealtime() const override;
   bool SupportsAutomation() const override;

   bool SaveSettings(
      const EffectSettings &settings, CommandParameters & parms) const override;
   bool LoadSettings(
      const CommandParameters & parms, EffectSettings &settings) const override;

   bool LoadUserPreset(
      const RegistryPath & name, EffectSettings &settings) const override;
   bool SaveUserPreset(
      const RegistryPath & name, const EffectSettings &settings) const override;

   RegistryPaths GetFactoryPresets() const override;
   bool LoadFactoryPreset(int id, EffectSettings &settings) const override;
   bool LoadFactoryDefaults(EffectSettings &settings) const override;

   unsigned GetAudioInCount() const override;
   unsigned GetAudioOutCount() const override;

   // VisitSettings(), SaveSettings(), and LoadSettings()
   // use the functions of EffectParameterMethods.  By default, this function
   // defines an empty list of parameters.
   virtual const EffectParameterMethods &Parameters() const;

   int ShowClientInterface(
      wxWindow &parent, wxDialog &dialog, bool forceModal = false) override;

   // EffectUIClientInterface implementation

   std::unique_ptr<EffectUIValidator> PopulateUI(
      ShuttleGui &S, EffectInstance &instance, EffectSettingsAccess &access)
   override;
   bool IsGraphicalUI() override;
   bool ValidateUI(EffectSettings &) override;
   bool CloseUI() override;

   bool CanExportPresets() override;
   void ExportPresets(const EffectSettings &settings) const override;
   void ImportPresets(EffectSettings &settings) override;

   bool HasOptions() override;
   void ShowOptions() override;

   // EffectPlugin implementation

   const EffectSettingsManager& GetDefinition() const override;
   virtual NumericFormatSymbol GetSelectionFormat() /* not override? */; // time format in Selection toolbar

   // EffectPlugin implementation

   int ShowHostInterface( wxWindow &parent,
      const EffectDialogFactory &factory,
      EffectInstance &instance, EffectSettingsAccess &access,
      bool forceModal = false) override;
   bool SaveSettingsAsString(
      const EffectSettings &settings, wxString & parms) const override;
   bool LoadSettingsFromString(
      const wxString & parms, EffectSettings &settings) const override;
   bool IsBatchProcessing() const override;
   void SetBatchProcessing() override;
   void UnsetBatchProcessing() override;
   bool TransferDataToWindow(const EffectSettings &settings) override;
   bool TransferDataFromWindow(EffectSettings &settings) override;

   // Effect implementation

   unsigned TestUIFlags(unsigned mask);

   //! Re-invoke DoEffect on another Effect object that implements the work
   bool Delegate(Effect &delegate, EffectSettings &settings);

   // Display a message box, using effect's (translated) name as the prefix
   // for the title.
   enum : long { DefaultMessageBoxStyle = wxOK | wxCENTRE };
   int MessageBox(const TranslatableString& message,
                  long style = DefaultMessageBoxStyle,
                  const TranslatableString& titleStr = {}) const;

   static void IncEffectCounter(){ nEffectsDone++;}

   bool EnableApply(bool enable = true);

 protected:
   
   bool EnablePreview(bool enable = true);

   //! Default implementation returns false
   bool CheckWhetherSkipEffect(const EffectSettings &settings) const override;

   //! Default implementation returns `previewLength`
   double CalcPreviewInputLength(
      const EffectSettings &settings, double previewLength) const override;

   //! Add controls to effect panel; always succeeds
   /*!
    @return if not null, then return it from Effect::PopulateUI instead of a
    DefaultEffectUIValidator; default implementation returns null
    */
   virtual std::unique_ptr<EffectUIValidator> PopulateOrExchange(
      ShuttleGui & S, EffectInstance &instance, EffectSettingsAccess &access);

   // No more virtuals!

   // The Progress methods all return true if the user has cancelled;
   // you should exit immediately if this happens (cleaning up memory
   // is okay, but don't try to undo).

   // Pass a fraction between 0.0 and 1.0
   bool TotalProgress(double frac, const TranslatableString & = {}) const;

   // Pass a fraction between 0.0 and 1.0, for the current track
   // (when doing one track at a time)
   bool TrackProgress(
      int whichTrack, double frac, const TranslatableString & = {}) const;

   // Pass a fraction between 0.0 and 1.0, for the current track group
   // (when doing stereo groups at a time)
   bool TrackGroupProgress(
      int whichGroup, double frac, const TranslatableString & = {}) const;

   int GetNumWaveTracks() const { return mNumTracks; }
   int GetNumWaveGroups() const { return mNumGroups; }

   // Calculates the start time and length in samples for one or two channels
   void GetBounds(
      const WaveTrack &track, const WaveTrack *pRight,
      sampleCount *start, sampleCount *len);

   // Use this method to copy the input tracks to mOutputTracks, if
   // doing the processing on them, and replacing the originals only on success (and not cancel).
   // If not all sync-locked selected, then only selected wave tracks.
   void CopyInputTracks(bool allSyncLockSelected = false);

   // For the use of analyzers, which don't need to make output wave tracks,
   // but may need to add label tracks.
   class AUDACITY_DLL_API AddedAnalysisTrack {
      friend Effect;
      AddedAnalysisTrack(Effect *pEffect, const wxString &name);
      AddedAnalysisTrack(const AddedAnalysisTrack&) PROHIBITED;

   public:

      AddedAnalysisTrack() {}

      // So you can have a vector of them
      AddedAnalysisTrack(AddedAnalysisTrack &&that);

      LabelTrack *get() const { return mpTrack; }

      // Call this to indicate successful completion of the analyzer.
      void Commit();

      // Destructor undoes the addition of the analysis track if not committed.
      ~AddedAnalysisTrack();

   private:
      Effect *mpEffect{};
      LabelTrack *mpTrack{};
   };

   // Set name to given value if that is not empty, else use default name
   std::shared_ptr<AddedAnalysisTrack> AddAnalysisTrack(const wxString &name = wxString());

   // For the use of analyzers, which don't need to make output wave tracks,
   // but may need to modify label tracks.
   class AUDACITY_DLL_API ModifiedAnalysisTrack {
      friend Effect;
      ModifiedAnalysisTrack
         (Effect *pEffect, const LabelTrack *pOrigTrack, const wxString &name);
      ModifiedAnalysisTrack(const ModifiedAnalysisTrack&) PROHIBITED;

   public:

      ModifiedAnalysisTrack();

      // So you can have a vector of them
      ModifiedAnalysisTrack(ModifiedAnalysisTrack &&that);

      LabelTrack *get() const { return mpTrack; }

      // Call this to indicate successful completion of the analyzer.
      void Commit();

      // Destructor undoes the modification of the analysis track if not committed.
      ~ModifiedAnalysisTrack();

   private:
      Effect *mpEffect{};
      LabelTrack *mpTrack{};
      std::shared_ptr<Track> mpOrigTrack{};
   };

   // Set name to given value if that is not empty, else use default name
   ModifiedAnalysisTrack ModifyAnalysisTrack
      (const LabelTrack *pOrigTrack, const wxString &name = wxString());

   // Use this to append a NEW output track.
   Track *AddToOutputTracks(const std::shared_ptr<Track> &t);

protected:
   // UI
   //! This smart pointer tracks the lifetime of the dialog
   wxWeakRef<wxDialog> mHostUIDialog;
   wxWindow       *mUIParent{};

public:
   wxWindow* GetUIParent() { return mUIParent; }

private:
   wxString GetSavedStateGroup();

   bool mIsBatch{ false };
};

//! Convenience for generating EffectDefinitionInterface overrides
//! and static down-casting functions
template<typename Settings, typename Base>
class EffectWithSettings : public Base {
public:
   EffectSettings MakeSettings() const override
   {
      return EffectSettings::Make<Settings>();
   }
   bool CopySettingsContents(
      const EffectSettings &src, EffectSettings &dst) const override
   {
      return EffectSettings::Copy<Settings>(src, dst);
   }
   //! Assume settings originated from MakeSettings() and copies thereof
   static inline Settings &GetSettings(EffectSettings &settings)
   {
      auto pSettings = settings.cast<Settings>();
      assert(pSettings);
      return *pSettings;
   }
   //! Assume settings originated from MakeSettings() and copies thereof
   static inline const Settings &GetSettings(const EffectSettings &settings)
   {
      return GetSettings(const_cast<EffectSettings &>(settings));
   }
   static inline Settings *
   FetchParameters(Base &, EffectSettings &s) {
      return &GetSettings(s);
   }
};

//! Subclass of Effect, to be eliminated after all of its subclasses
//! are rewritten to be stateless
class StatefulEffect
   : public StatefulEffectBase
   , public Effect
{
public:
   class AUDACITY_DLL_API Instance : public StatefulEffectBase::Instance {
   public:
      using StatefulEffectBase::Instance::Instance;
      bool Process(EffectSettings &settings) override;
   };
   std::shared_ptr<EffectInstance> MakeInstance(EffectSettings &settings)
       const override;
};

// FIXME:
// FIXME:  Remove this once all effects are using the NEW dialog
// FIXME:

#define ID_EFFECT_PREVIEW ePreviewID

#endif
