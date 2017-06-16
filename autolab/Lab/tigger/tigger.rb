require "AssessmentBase.rb"

module Tigger
  include AssessmentBase

  def assessmentInitialize(course)
    super("tigger",course)
    @problems = []
  end

end
